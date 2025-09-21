/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipeline.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:12:05 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/22 02:27:16 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>

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
			gc_exit(1);
		}
		close(pipefd[0]);
		close(pipefd[1]);
		gc_exit(exec_ast(left));
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
			gc_exit(1);
		}
		close(pipefd[1]);
		close(pipefd[0]);
		gc_exit(exec_ast(right));
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

int	exec_pipeline(const t_ast_node *left, const t_ast_node *right)
{
	int		pipefd[2];
	pid_t	lpid;
	pid_t	rpid;

	if (sh_signal_interrupted())
	{
		reset_signals();
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
