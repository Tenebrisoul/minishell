/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_exec.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:24:23 by root              #+#    #+#             */
/*   Updated: 2025/09/19 15:14:03 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	wait_for_child(pid_t pid)
{
	int	wstatus;

	wstatus = 0;
	while (waitpid(pid, &wstatus, 0) < 0)
	{
		if (errno != EINTR)
		{
			perror("waitpid");
			return (1);
		}
	}
	if (WIFEXITED(wstatus))
		return (WEXITSTATUS(wstatus));
	if (WIFSIGNALED(wstatus))
		return (128 + WTERMSIG(wstatus));
	return (1);
}

int	exec_builtin_with_redir(const t_command *cmd, char **argv)
{
	pid_t	bpid;

	bpid = fork();
	if (bpid < 0)
	{
		perror("fork");
		return (1);
	}
	if (bpid == 0)
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		if (apply_redirs(cmd->redirects) != 0)
			get_env()->exit_status = 1;
		else
			get_env()->exit_status = run_builtin(argv);
		exit(-1);
	}
	return (wait_for_child(bpid));
}

static int	handle_exec_error(const t_command *cmd, char *exe)
{
	struct stat	st;

	if (!exe)
	{
		write(2, "minishell: ", 11);
		write(2, cmd->args[0], sh_strlen(cmd->args[0]));
		write(2, ": command not found\n", 20);
		return (127);
	}
	if (stat(exe, &st) == 0 && S_ISDIR(st.st_mode))
	{
		write(2, "minishell: ", 11);
		write(2, cmd->args[0], sh_strlen(cmd->args[0]));
		write(2, ": Is a directory\n", 17);
		return (126);
	}
	return (0);
}

static void	handle_execve_error(const t_command *cmd)
{
	write(2, "minishell: ", 11);
	write(2, cmd->args[0], sh_strlen(cmd->args[0]));
	if (errno == ENOENT)
		write(2, ": No such file or directory\n", 28);
	else
		write(2, ": Permission denied\n", 20);
}

int	exec_child_process(const t_command *cmd, char **argv)
{
	char	*exe;
	char	**env_array;
	int		error_code;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	if (apply_redirs(cmd->redirects) != 0)
		return (1);
	exe = find_exec_in_path(argv[0]);
	error_code = handle_exec_error(cmd, exe);
	if (error_code != 0)
		return (error_code);
	env_array = get_env_array();
	execve(exe, argv, env_array);
	handle_execve_error(cmd);
	if (errno == ENOENT)
		return (127);
	return (126);
}

