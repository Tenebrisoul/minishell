/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:12:11 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/22 02:20:58 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include <stdio.h>

int	apply_redirs_with_restore(const t_redirect *r, int *saved_stdin,
		int *saved_stdout)
{
	*saved_stdin = dup(0);
	*saved_stdout = dup(1);
	if (*saved_stdin < 0 || *saved_stdout < 0)
	{
		if (*saved_stdin >= 0)
			close(*saved_stdin);
		if (*saved_stdout >= 0)
			close(*saved_stdout);
		return (1);
	}
	apply_redirs(r);
	return (0);
}

void	restore_fds(int saved_stdin, int saved_stdout)
{
	if (saved_stdin >= 0)
	{
		dup2(saved_stdin, 0);
		close(saved_stdin);
	}
	if (saved_stdout >= 0)
	{
		dup2(saved_stdout, 1);
		close(saved_stdout);
	}
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
		gc_exit(exec_child_process(cmd, argv));
	return (wait_for_child(pid));
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
