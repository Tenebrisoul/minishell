/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:24:23 by root              #+#    #+#             */
/*   Updated: 2025/09/19 15:14:03 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	update_underscore_var(const t_command *cmd)
{
	t_env_item	*underscore_item;
	char		*last_arg;

	if (!cmd || cmd->argc <= 0 || !cmd->args)
		return ;
	last_arg = cmd->args[cmd->argc - 1];
	if (!last_arg)
		return ;
	underscore_item = new_env_item("_", last_arg);
	if (underscore_item)
		add_env_item(underscore_item);
}

static int	exec_command(const t_command *cmd)
{
	int	rc;

	if (cmd->argc <= 0)
		return (0);
	if (sh_signal_interrupted())
	{
		sh_signal_reset();
		return (130);
	}
	expand_args(cmd);
	expand_redirects(cmd);
	if (!cmd->args[0] || !cmd->args[0][0])
		return (0);
	update_underscore_var(cmd);
	if (is_builtin(cmd->args[0]))
	{
		if (cmd->redirects)
			return (exec_builtin_with_redir(cmd, cmd->args));
		else
		{
			rc = run_builtin(cmd->args);
			return (rc);
		}
	}
	return (exec_external_command(cmd, cmd->args));
}

int	exec_external_command(const t_command *cmd, char **argv)
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return (1);
	}
	if (pid == 0)
	{
		get_env()->exit_status = exec_child_process(cmd, argv);
		exit(-1);
	}
	return (wait_for_child(pid));
}

static int	setup_pipe_left(const t_ast_node *left, int *pipefd)
{
	pid_t	lpid;

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
			get_env()->exit_status = 1;
			exit(-1);
		}
		close(pipefd[0]);
		close(pipefd[1]);
		get_env()->exit_status = exec_ast(left);
		exit(-1);
	}
	return (lpid);
}

static int	setup_pipe_right(const t_ast_node *right, int *pipefd, pid_t lpid)
{
	pid_t	rpid;

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
			get_env()->exit_status = 1;
			exit(-1);
		}
		close(pipefd[1]);
		close(pipefd[0]);
		get_env()->exit_status = exec_ast(right);
		exit(-1);
	}
	return (rpid);
}

static int	wait_for_pipeline(pid_t lpid, pid_t rpid)
{
	int	status;

	status = 0;
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

static int	exec_pipeline(const t_ast_node *left, const t_ast_node *right)
{
	int		pipefd[2];
	pid_t	lpid;
	pid_t	rpid;

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
	return (wait_for_pipeline(lpid, rpid));
}

static int	exec_sequence(const t_ast_node *left, const t_ast_node *right)
{
	int	left_status;

	if (sh_signal_interrupted())
	{
		sh_signal_reset();
		return (130);
	}
	left_status = exec_ast(left);
	get_env()->exit_status = left_status;
	if (sh_signal_interrupted())
	{
		sh_signal_reset();
		return (130);
	}
	return (exec_ast(right));
}

int	exec_ast(const t_ast_node *ast)
{
	int	result;

	if (!ast)
		return (0);
	sh_signal_set_state(STATE_COMMAND, 1);
	result = 0;
	if (ast->type == NODE_COMMAND)
		result = exec_command(ast->u_data.command);
	else if (ast->type == NODE_SEQUENCE)
		result = exec_sequence(ast->u_data.s_binary.left,
				ast->u_data.s_binary.right);
	else if (ast->type == NODE_PIPELINE)
		result = exec_pipeline(ast->u_data.s_binary.left,
				ast->u_data.s_binary.right);
	sh_signal_set_state(STATE_COMMAND, 0);
	return (result);
}
