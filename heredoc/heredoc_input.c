/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_input.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:11:17 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/22 01:23:01 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include <readline/readline.h>

void	heredoc_queue_expandables(void)
{
	t_expander	*expander;

	expander = get_expander(GET);
	while (expander->prompt[expander->marker])
	{
		if (expander->prompt[expander->marker] == '$'
			&& !is_escaped(expander->prompt, expander->marker))
			heredoc_insert_curr_to_queue(expander);
		expander->marker++;
	}
}

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

static char	*handle_non_tty_input(void)
{
	char	*line;
	size_t	len;
	ssize_t	read_len;

	len = 0;
	line = NULL;
	read_len = getline(&line, &len, stdin);
	if (read_len == -1)
	{
		return (NULL);
	}
	if (read_len > 0 && line[read_len - 1] == '\n')
		line[read_len - 1] = '\0';
	insert_to_gc(new_trash(line), GC_GC);
	return (line);
}

char	*get_heredoc_line(void)
{
	char	*line;

	if (isatty(STDIN_FILENO))
	{
		if (sh_signal_interrupted())
			return (NULL);
		line = safe_heredoc("> ");
		if (!line || sh_signal_interrupted())
			return (NULL);
		return (line);
	}
	return (handle_non_tty_input());
}

void	print_heredoc_warning(char *delimiter)
{
	write(2, "minishell: warning: here-document ", 35);
	write(2, "delimited by end-of-file (wanted `", 35);
	write(2, delimiter, sh_strlen(delimiter));
	write(2, "')\n", 3);
}
