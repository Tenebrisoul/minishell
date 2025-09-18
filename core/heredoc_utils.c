/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/01 00:00:00 by student           #+#    #+#             */
/*   Updated: 2024/01/01 00:00:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdio.h>
#include <errno.h>

void	copy_content(char *dest, char *src, int len)
{
	int	i;

	i = 0;
	while (i < len)
	{
		dest[i] = src[i];
		i++;
	}
}

char	*resize_content(char *content, int *cap, int len, int line_len)
{
	char	*new_content;

	while (len + line_len + 2 > *cap)
	{
		*cap *= 2;
		new_content = (char *)alloc(*cap);
		if (!new_content)
			return (content);
		copy_content(new_content, content, len);
		content = new_content;
	}
	return (content);
}

void	add_line_to_content(char **content, int *content_len,
	char *expanded, int line_len)
{
	int	i;

	i = 0;
	while (i < line_len)
	{
		(*content)[(*content_len)++] = expanded[i];
		i++;
	}
	(*content)[(*content_len)++] = '\n';
	(*content)[*content_len] = '\0';
}

int	check_delimiter(char *line, const char *delimiter, int delim_len)
{
	if (sh_strlen(line) == delim_len
		&& sh_strncmp(line, delimiter, delim_len) == 0)
	{
		free(line);
		sh_signal_set_state(STATE_HEREDOC, 0);
		return (1);
	}
	return (0);
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

char	*get_heredoc_line(void)
{
	char		*line;
	size_t		len;
	ssize_t		read_len;
	int			stdin_fd;

	if (isatty(STDIN_FILENO))
	{
		if (sh_signal_interrupted())
			return (NULL);
		line = safe_heredoc("> ");
		if (!line || sh_signal_interrupted())
		{
			return (NULL);
		}
		return (line);
	}
	len = 0;
	line = NULL;
	read_len = getline(&line, &len, stdin);
	if (read_len == -1)
	{
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
		return (NULL);
	}
	if (read_len > 0 && line[read_len - 1] == '\n')
		line[read_len - 1] = '\0';
	return (line);
}
