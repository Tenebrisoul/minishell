#include "shell.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>

static int apply_redirs(t_shell *sh, const t_redirect *r)
{
    int fd;
    while (r) {
        if (r->type == REDIR_IN) {
            fd = open(r->filename, O_RDONLY);
            if (fd < 0) { perror(r->filename); return 1; }
            if (dup2(fd, 0) < 0) { perror("dup2"); close(fd); return 1; }
            close(fd);
        } else if (r->type == REDIR_OUT) {
            fd = open(r->filename, O_WRONLY|O_CREAT|O_TRUNC, 0644);
            if (fd < 0) { perror(r->filename); return 1; }
            if (dup2(fd, 1) < 0) { perror("dup2"); close(fd); return 1; }
            close(fd);
        } else if (r->type == REDIR_APPEND) {
            fd = open(r->filename, O_WRONLY|O_CREAT|O_APPEND, 0644);
            if (fd < 0) { perror(r->filename); return 1; }
            if (dup2(fd, 1) < 0) { perror("dup2"); close(fd); return 1; }
            close(fd);
        } else if (r->type == REDIR_HEREDOC) {
            /* Handle heredoc redirection */
            int heredoc_fd = handle_heredoc(sh, r->filename);
            if (heredoc_fd < 0) {
                write(2, "minishell: heredoc error\n", 25);
                return 1;
            }
            if (dup2(heredoc_fd, 0) < 0) { 
                perror("dup2"); 
                close(heredoc_fd); 
                return 1; 
            }
            close(heredoc_fd);
        }
        r = r->next;
    }
    return 0;
}

static char *find_exec_in_path(t_shell *sh, const char *file)
{
    const char *path = sh_getenv_val(sh, "PATH");
    char **dirs; int i;
    if (!file || !*file) return NULL;
    if (sh_strchr(file, '/'))
        return sh_strdup(file);
    if (!path) return NULL;
    dirs = sh_split_colon(path);
    if (!dirs) return NULL;
    for (i=0; dirs[i]; ++i) {
        char *full = sh_join_path(dirs[i], file);
        if (!full) break;
        if (access(full, X_OK) == 0)
        { sh_free_strarray(dirs); return full; }
        /* Don't free full - it's gc_malloc'd */
    }
    sh_free_strarray(dirs);
    return NULL;
}

static int exec_command(t_shell *sh, const t_command *cmd)
{
    if (cmd->argc <= 0) return 0;
    char **argv = expand_argv(sh, cmd->args, cmd->argc);
    int i;
    if (!argv) return 1;
    for (i=0;i<cmd->argc;i++){
        char *u = unquote_string(sh, argv[i]);
        if (u){ argv[i]=u; }
    }
    const t_redirect *rr=cmd->redirects; while (rr){
        if (rr->filename){
            char *e=expand_string(sh, rr->filename);
            if (e){ ((t_redirect*)rr)->filename=e; char *u=unquote_string(sh, rr->filename); if(u){ ((t_redirect*)rr)->filename=u; } }
        }
        rr=rr->next;
    }
    if (is_builtin(argv[0])){
        if (cmd->redirects){
            pid_t bpid = fork();
            if (bpid < 0) { perror("fork"); sh_free_strarray(argv); return 1; }
            if (bpid == 0) {
                signal(SIGINT, SIG_DFL);
                signal(SIGQUIT, SIG_DFL);
                if (apply_redirs(sh, cmd->redirects) != 0) exit(1);
                int rc = run_builtin(sh, argv);
                exit(rc);
            }
            int w=0; if (waitpid(bpid, &w, 0) < 0){ perror("waitpid"); sh_free_strarray(argv); return 1; }
            sh_free_strarray(argv);
            if (WIFEXITED(w)) return WEXITSTATUS(w);
            if (WIFSIGNALED(w)) return 128 + WTERMSIG(w);
            return 1;
        } else {
            int rc = run_builtin(sh, argv);
            sh_free_strarray(argv);
            return rc;
        }
    }

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }
    if (pid == 0) {
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
    if (apply_redirs(sh, cmd->redirects) != 0)
            exit(1);
        char *exe = find_exec_in_path(sh, argv[0]);
        if (!exe) { 
            write(2, "minishell: ", 11);
            write(2, cmd->args[0], sh_strlen(cmd->args[0]));
            write(2, ": command not found\n", 20);
            exit(127); 
        }
        execve(exe, argv, sh->env);
        write(2, "minishell: ", 11);
        write(2, cmd->args[0], sh_strlen(cmd->args[0]));
        write(2, ": Permission denied\n", 20);
        exit(126);
    }
    int wstatus=0; if (waitpid(pid, &wstatus, 0) < 0) { perror("waitpid"); return 1; }
    sh_free_strarray(argv);
    if (WIFEXITED(wstatus)) return WEXITSTATUS(wstatus);
    if (WIFSIGNALED(wstatus)) return 128 + WTERMSIG(wstatus);
    return 1;
}

static int exec_pipeline(t_shell *sh, const t_ast_node *left, const t_ast_node *right)
{
    int pipefd[2]; pid_t lpid, rpid; int status=0;
    if (pipe(pipefd) < 0) { perror("pipe"); return 1; }
    lpid = fork();
    if (lpid < 0) { perror("fork"); close(pipefd[0]); close(pipefd[1]); return 1; }
    if (lpid == 0) {
        if (dup2(pipefd[1], 1) < 0) { perror("dup2"); exit(1); }
        close(pipefd[0]); close(pipefd[1]);
        int rc = exec_ast(sh, left);
        exit(rc);
    }
    rpid = fork();
    if (rpid < 0) { perror("fork"); kill(lpid, SIGKILL); close(pipefd[0]); close(pipefd[1]); return 1; }
    if (rpid == 0) {
        if (dup2(pipefd[0], 0) < 0) { perror("dup2"); exit(1); }
        close(pipefd[1]); close(pipefd[0]);
        int rc = exec_ast(sh, right);
        exit(rc);
    }
    close(pipefd[0]); close(pipefd[1]);
    waitpid(lpid, NULL, 0);
    if (waitpid(rpid, &status, 0) < 0) { perror("waitpid"); return 1; }
    if (WIFEXITED(status)) return WEXITSTATUS(status);
    if (WIFSIGNALED(status)) return 128 + WTERMSIG(status);
    return 1;
}

static int exec_sequence(t_shell *sh, const t_ast_node *left, const t_ast_node *right)
{
	int left_status = exec_ast(sh, left);
	if (sh) sh->last_status = left_status;
	return exec_ast(sh, right);
}

int exec_ast(t_shell *sh, const t_ast_node *ast)
{
    if (!ast) return 0;
    if (ast->type == NODE_COMMAND)
        return exec_command(sh, ast->u_data.command);
    if (ast->type == NODE_SEQUENCE)
        return exec_sequence(sh, ast->u_data.s_sequence.left, ast->u_data.s_sequence.right);
    return exec_pipeline(sh, ast->u_data.s_pipeline.left, ast->u_data.s_pipeline.right);
}
