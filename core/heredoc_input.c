/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_input.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/01 00:00:00 by student           #+#    #+#             */
/*   Updated: 2025/09/19 00:59:49 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	handle_getline_error(char *line)
{
	int	stdin_fd;

	if (line)
		free(line);
	if (sh_signal_interrupted())
	{
		stdin_fd = open("/dev/tty", O_RDONLY);
		if (stdin_fd >= 0)
		{
			dup2(stdin_fd, STDIN_FILENO);
			close(stdin_fd);
		}
	}
}

static char	*handle_non_tty_input(void)
{
	char		*line;
	size_t		len;
	ssize_t		read_len;

	len = 0;
	line = NULL;
	read_len = getline(&line, &len, stdin);
	if (read_len == -1)
	{
		handle_getline_error(line);
		return (NULL);
	}
	if (read_len > 0 && line[read_len - 1] == '\n')
		line[read_len - 1] = '\0';
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

char	*safe_heredoc(char *prompt)
{
	int		saved_in;
	int		saved_out;
	int		tty;
	char	*line;

	redirect_tty(&tty, &saved_in, &saved_out);
	line = readline(prompt);
	insert_to_gc(new_trash(line));
	if (tty >= 0)
		restore_stdio(saved_in, saved_out);
	else
	{
		close(saved_in);
		close(saved_out);
	}
	return (line);
}

void	print_heredoc_warning(char *delimiter)
{
	write(2, "minishell: warning: here-document ", 35);
	write(2, "delimited by end-of-file (wanted `", 35);
	write(2, delimiter, sh_strlen(delimiter));
	write(2, "')\n", 3);
}
