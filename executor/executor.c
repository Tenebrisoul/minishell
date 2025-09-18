#include "../minishell.h"
#include <errno.h>

static int apply_redir_in(const t_redirect *r)
{
	int fd;

	fd = open(r->filename, O_RDONLY);
	if (fd < 0)
	{
		perror(r->filename);
		return (1);
	}
	if (dup2(fd, 0) < 0)
	{
		perror("dup2");
		close(fd);
		return (1);
	}
	close(fd);
	return (0);
}

static int apply_redir_out(const t_redirect *r)
{
	int fd;

	fd = open(r->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
	{
		perror(r->filename);
		return (1);
	}
	if (dup2(fd, 1) < 0)
	{
		perror("dup2");
		close(fd);
		return (1);
	}
	close(fd);
	return (0);
}

static int apply_redir_append(const t_redirect *r)
{
	int fd;

	fd = open(r->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd < 0)
	{
		perror(r->filename);
		return (1);
	}
	if (dup2(fd, 1) < 0)
	{
		perror("dup2");
		close(fd);
		return (1);
	}
	close(fd);
	return (0);
}

static int apply_redir_heredoc(const t_redirect *r)
{
	int pipefd[2];
	int content_len;

	if (pipe(pipefd) < 0)
	{
		perror("pipe");
		return (1);
	}
	content_len = sh_strlen(r->filename);
	if (write(pipefd[1], r->filename, content_len) < 0)
	{
		perror("write");
		close(pipefd[0]);
		close(pipefd[1]);
		return (1);
	}
	close(pipefd[1]);
	if (dup2(pipefd[0], 0) < 0)
	{
		perror("dup2");
		close(pipefd[0]);
		return (1);
	}
	close(pipefd[0]);
	return (0);
}

static int apply_redirs(const t_redirect *r)
{
	while (r)
	{
		if (r->type == REDIR_IN && apply_redir_in(r))
			return (1);
		else if (r->type == REDIR_OUT && apply_redir_out(r))
			return (1);
		else if (r->type == REDIR_APPEND && apply_redir_append(r))
			return (1);
		else if (r->type == REDIR_HEREDOC && apply_redir_heredoc(r))
			return (1);
		r = r->next;
	}
	return (0);
}

static char *check_direct_path(const char *file)
{
	if (!file || !*file)
		return (NULL);
	if (sh_strchr(file, '/'))
		return (sh_strdup(file));
	return (NULL);
}

static char *search_in_dirs(char **dirs, const char *file)
{
	int i;
	char *full;

	i = 0;
	while (dirs[i])
	{
		full = sh_join_path(dirs[i], file);
		if (!full)
			break ;
		if (access(full, X_OK) == 0)
		{
			sh_free_strarray(dirs);
			return (full);
		}
		i++;
	}
	return (NULL);
}

static char *find_exec_in_path(const char *file)
{
	const char *path;
	char **dirs;
	char *result;

	result = check_direct_path(file);
	if (result)
		return (result);
	path = sh_getenv_val("PATH");
	if (!path)
		return (NULL);
	dirs = sh_split_colon(path);
	if (!dirs)
		return (NULL);
	result = search_in_dirs(dirs, file);
	sh_free_strarray(dirs);
	return (result);
}

static void	expand_args(const t_command *cmd)
{
	int i;
	char *expanded;
	char **wildcard_matches;
	int j;

	if (!cmd || !cmd->args)
		return ;
	i = 0;
	while (i < cmd->argc && cmd->args[i])
	{
		// First expand variables
		expanded = expand(cmd->args[i]);
		if (expanded)
			(cmd)->args[i] = expanded;

		// Then expand wildcards if present
		if (sh_strchr(cmd->args[i], '*'))
		{
			wildcard_matches = wildcard_expand(cmd->args[i]);
			if (wildcard_matches && wildcard_matches[0])
			{
				// Replace the argument with first match
				(cmd)->args[i] = wildcard_matches[0];

				// TODO: Handle multiple matches by expanding args array
				// For now, just use the first match
				j = 1;
				while (wildcard_matches[j])
				{
					// Add additional matches to args array
					// This would require reallocating args array
					j++;
				}
			}
		}
		i++;
	}
}

static void expand_redirects(const t_command *cmd)
{
	const t_redirect *rr;
	char *e;

	rr = cmd->redirects;
	while (rr)
	{
		if (rr->filename && rr->type != REDIR_HEREDOC)
		{
			e = expand(rr->filename);
			if (e)
				((t_redirect *)rr)->filename = e;
		}
		rr = rr->next;
	}
}

static int exec_builtin_with_redir(const t_command *cmd, char **argv)
{
	pid_t bpid;
	int w;

	bpid = fork();
	if (bpid < 0)
	{
		perror("fork");
		sh_free_strarray(argv);
		return (1);
	}
	if (bpid == 0)
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		if (apply_redirs(cmd->redirects) != 0)
			exit(1);
		exit(run_builtin(argv));
	}
	w = 0;
	while (waitpid(bpid, &w, 0) < 0)
	{
		if (errno != EINTR)
		{
			perror("waitpid");
			sh_free_strarray(argv);
			return (1);
		}
	}
	sh_free_strarray(argv);
	if (WIFEXITED(w))
		return (WEXITSTATUS(w));
	if (WIFSIGNALED(w))
		return (128 + WTERMSIG(w));
	return (1);
}

static void exec_child_process(const t_command *cmd, char **argv)
{
	char *exe;
	char **env_array;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	if (apply_redirs(cmd->redirects) != 0)
		exit(1);
	exe = find_exec_in_path(argv[0]);
	if (!exe)
	{
		write(2, cmd->args[0], sh_strlen(cmd->args[0]));
		write(2, ": command not found\n", 20);
		exit(127);
	}
	env_array = get_env_array();
	execve(exe, argv, env_array);
	write(2, "minishell: ", 11);
	write(2, cmd->args[0], sh_strlen(cmd->args[0]));
	write(2, ": Permission denied\n", 20);
	exit(126);
}

static int exec_external_command(const t_command *cmd, char **argv)
{
	pid_t pid;
	int wstatus;

	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return (1);
	}
	if (pid == 0)
		exec_child_process(cmd, argv);
	wstatus = 0;
	while (waitpid(pid, &wstatus, 0) < 0)
	{
		if (errno != EINTR)
		{
			perror("waitpid");
			return (1);
		}
	}
	sh_free_strarray(argv);
	if (WIFEXITED(wstatus))
		return (WEXITSTATUS(wstatus));
	if (WIFSIGNALED(wstatus))
		return (128 + WTERMSIG(wstatus));
	return (1);
}

static int exec_command(const t_command *cmd)
{
	int rc;

	if (cmd->argc <= 0)
		return (0);
	if (sh_signal_interrupted())
	{
		sh_signal_reset();
		return (130);
	}
	expand_args(cmd);
	expand_redirects(cmd);
	if (is_builtin(cmd->args[0]))
	{
		if (cmd->redirects)
			return (exec_builtin_with_redir(cmd, cmd->args));
		else
		{
			rc = run_builtin(cmd->args);
			sh_free_strarray(cmd->args);
			return (rc);
		}
	}
	return (exec_external_command(cmd, cmd->args));
}

static int setup_pipe_left(const t_ast_node *left, int *pipefd)
{
	pid_t lpid;

	lpid = fork();
	if (lpid < 0)
	{
		perror("fork");
		close(pipefd[0]);
		close(pipefd[1]);
		return (-1);
	}
	if (lpid == 0)
	{
		if (dup2(pipefd[1], 1) < 0)
		{
			perror("dup2");
			exit(1);
		}
		close(pipefd[0]);
		close(pipefd[1]);
		exit(exec_ast(left));
	}
	return (lpid);
}

static int setup_pipe_right(const t_ast_node *right, int *pipefd, pid_t lpid)
{
	pid_t rpid;

	rpid = fork();
	if (rpid < 0)
	{
		perror("fork");
		kill(lpid, SIGKILL);
		close(pipefd[0]);
		close(pipefd[1]);
		return (-1);
	}
	if (rpid == 0)
	{
		if (dup2(pipefd[0], 0) < 0)
		{
			perror("dup2");
			exit(1);
		}
		close(pipefd[1]);
		close(pipefd[0]);
		exit(exec_ast(right));
	}
	return (rpid);
}

static int exec_pipeline(const t_ast_node *left, const t_ast_node *right)
{
	int pipefd[2];
	pid_t lpid;
	pid_t rpid;
	int status;

	status = 0;
	if (sh_signal_interrupted())
	{
		sh_signal_reset();
		return (130);
	}
	if (pipe(pipefd) < 0)
	{
		perror("pipe");
		return (1);
	}
	lpid = setup_pipe_left(left, pipefd);
	if (lpid < 0)
		return (1);
	rpid = setup_pipe_right(right, pipefd, lpid);
	if (rpid < 0)
		return (1);
	close(pipefd[0]);
	close(pipefd[1]);
	while (waitpid(lpid, NULL, 0) < 0 && errno == EINTR)
		;
	while (waitpid(rpid, &status, 0) < 0)
	{
		if (errno != EINTR)
		{
			perror("waitpid");
			return (1);
		}
	}
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}

static int exec_sequence(const t_ast_node *left, const t_ast_node *right)
{
	if (sh_signal_interrupted()) {
		sh_signal_reset();
		return 130;
	}
	int left_status = exec_ast(left);
	get_env()->exit_status = left_status;
	if (sh_signal_interrupted()) {
		sh_signal_reset();
		return 130;
	}
	
	return exec_ast(right);
}

int exec_ast(const t_ast_node *ast)
{
    if (!ast) return 0;
    sh_signal_set_state(STATE_COMMAND, 1);
    int result = 0;
    if (ast->type == NODE_COMMAND)
        result = exec_command(ast->u_data.command);
    else if (ast->type == NODE_SEQUENCE)
        result = exec_sequence(ast->u_data.s_binary.left, ast->u_data.s_binary.right);
    else if (ast->type == NODE_PIPELINE)
        result = exec_pipeline(ast->u_data.s_binary.left, ast->u_data.s_binary.right);
    else if (ast->type == NODE_AND)
        result = execute_logical_and(ast, NULL);
    else if (ast->type == NODE_OR)
        result = execute_logical_or(ast, NULL);
    else if (ast->type == NODE_SUBSHELL)
        result = execute_subshell(ast, NULL);

    sh_signal_set_state(STATE_COMMAND, 0);

    return result;
}
