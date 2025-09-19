/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_redirs.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:24:23 by root              #+#    #+#             */
/*   Updated: 2025/09/19 15:14:03 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	apply_redir_in(const t_redirect *r)
{
	int	fd;

	fd = open(r->filename, O_RDONLY);
	if (fd < 0)
	{
		write(2, "minishell: ", 11);
		write(2, r->filename, sh_strlen(r->filename));
		write(2, ": No such file or directory\n", 28);
		return (1);
	}
	if (dup2(fd, 0) < 0)
	{
		write(2, "minishell: dup2: ", 17);
		perror("");
		close(fd);
		return (1);
	}
	close(fd);
	return (0);
}

static int	apply_redir_out(const t_redirect *r)
{
	int	fd;

	fd = open(r->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
	{
		write(2, "minishell: ", 11);
		write(2, r->filename, sh_strlen(r->filename));
		write(2, ": Permission denied\n", 20);
		return (1);
	}
	if (dup2(fd, 1) < 0)
	{
		write(2, "minishell: dup2: ", 17);
		perror("");
		close(fd);
		return (1);
	}
	close(fd);
	return (0);
}

static int	apply_redir_append(const t_redirect *r)
{
	int	fd;

	fd = open(r->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd < 0)
	{
		write(2, "minishell: ", 11);
		write(2, r->filename, sh_strlen(r->filename));
		write(2, ": Permission denied\n", 20);
		return (1);
	}
	if (dup2(fd, 1) < 0)
	{
		write(2, "minishell: dup2: ", 17);
		perror("");
		close(fd);
		return (1);
	}
	close(fd);
	return (0);
}

static int	apply_redir_heredoc(const t_redirect *r)
{
	int	pipefd[2];
	int	content_len;

	if (pipe(pipefd) < 0)
	{
		write(2, "minishell: pipe: ", 17);
		perror("");
		return (1);
	}
	content_len = sh_strlen(r->filename);
	if (write(pipefd[1], r->filename, content_len) < 0)
	{
		write(2, "minishell: write: ", 18);
		perror("");
		close(pipefd[0]);
		close(pipefd[1]);
		return (1);
	}
	close(pipefd[1]);
	if (dup2(pipefd[0], 0) < 0)
	{
		write(2, "minishell: dup2: ", 17);
		perror("");
		close(pipefd[0]);
		return (1);
	}
	close(pipefd[0]);
	return (0);
}

int	apply_redirs(const t_redirect *r)
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