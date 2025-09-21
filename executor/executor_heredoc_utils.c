/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_heredoc_utils.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:12:00 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/22 01:52:45 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	write_heredoc_content(int pipefd[2], const char *content)
{
	int	content_len;

	content_len = sh_strlen(content);
	if (write(pipefd[1], content, content_len) < 0)
	{
		write(2, "minishell: write: ", 18);
		perror("");
		close(pipefd[0]);
		close(pipefd[1]);
		return (1);
	}
	close(pipefd[1]);
	return (0);
}
