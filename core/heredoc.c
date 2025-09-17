/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                        :+:      :+:    :+:   */
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

static char	*get_heredoc_line(void)
{
	char		*line;
	size_t		len;
	ssize_t		read_len;

	if (isatty(STDIN_FILENO))
		return (readline("> "));
	len = 0;
	read_len = getline(&line, &len, stdin);
	if (read_len == -1)
	{
		if (line)
			free(line);
		return (NULL);
	}
	if (read_len > 0 && line[read_len - 1] == '\n')
		line[read_len - 1] = '\0';
	return (line);
}

static void	copy_content(char *dest, char *src, int len)
{
	int	i;

	i = 0;
	while (i < len)
	{
		dest[i] = src[i];
		i++;
	}
}

static char	*resize_content(char *content, int *cap, int len, int line_len)
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

static int	check_delimiter(char *line, const char *delimiter, int delim_len)
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

static void	add_line_to_content(char **content, int *content_len,
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

char	*read_heredoc_input(const char *delimiter)
{
	char	*content;
	int		content_len;
	int		content_cap;
	char	*line;
	int		delimiter_len;

	content = (char *)alloc(1024);
	if (!content)
		return (NULL);
	content[0] = '\0';
	content_len = 0;
	content_cap = 1024;
	delimiter_len = sh_strlen(delimiter);
	sh_signal_set_state(STATE_HEREDOC, 1);
	while (1)
	{
		if (sh_signal_interrupted())
		{
			sh_signal_set_state(STATE_HEREDOC, 0);
			sh_signal_reset();
			return (NULL);
		}
		line = get_heredoc_line();
		if (!line)
		{
			if (sh_signal_interrupted())
			{
				sh_signal_set_state(STATE_HEREDOC, 0);
				sh_signal_reset();
				return (NULL);
			}
			sh_signal_set_state(STATE_HEREDOC, 0);
			break ;
		}
		if (check_delimiter(line, delimiter, delimiter_len))
			break ;
		content = process_heredoc_line(content, &content_len, &content_cap, line);
		if (!content)
			return (NULL);
	}
	return (content);
}

char	*process_heredoc_line(char *content, int *content_len,
	int *content_cap, char *line)
{
	char	*expanded;
	int		line_len;

	expanded = expand(line);
	if (!expanded)
		expanded = line;
	else
		free(line);
	line_len = sh_strlen(expanded);
	content = resize_content(content, content_cap, *content_len, line_len);
	add_line_to_content(&content, content_len, expanded, line_len);
	if (expanded != line)
		free(expanded);
	return (content);
}

int	handle_heredoc(const char *delimiter)
{
	char	*content;
	int		pipefd[2];
	int		content_len;

	content = read_heredoc_input(delimiter);
	if (!content)
		return (-1);
	if (pipe(pipefd) < 0)
	{
		perror("pipe");
		return (-1);
	}
	content_len = sh_strlen(content);
	if (write(pipefd[1], content, content_len) < 0)
	{
		perror("write");
		close(pipefd[0]);
		close(pipefd[1]);
		return (-1);
	}
	close(pipefd[1]);
	return (pipefd[0]);
}