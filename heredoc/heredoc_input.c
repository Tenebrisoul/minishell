/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_input.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:11:17 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/22 01:53:45 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*safe_heredoc(char *prompt)
{
	int		saved_in;
	int		saved_out;
	char	*line;

	setup_tty(&saved_in, &saved_out);
	line = readline(prompt);
	insert_to_gc(new_trash(line), GC_GC);
	restore_tty(saved_in, saved_out);
	return (line);
}

char	*get_heredoc_line(void)
{
	char	*line;

	if (sh_signal_interrupted())
		return (NULL);
	line = safe_heredoc("> ");
	if (!line || sh_signal_interrupted())
		return (NULL);
	return (line);
}

void	print_heredoc_warning(char *delimiter)
{
	write(2, "minishell: warning: here-document ", 35);
	write(2, "delimited by end-of-file (wanted `", 35);
	write(2, delimiter, sh_strlen(delimiter));
	write(2, "')\n", 3);
}
