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

int	has_quotes(const char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] == '"' || str[i] == '\'')
			return (1);
		i++;
	}
	return (0);
}

char	*resize_content(char *content, int *cap, int len, int line_len)
{
	char	*new_content;
	int		i;

	while (len + line_len + 2 > *cap)
	{
		*cap *= 2;
		new_content = (char *)alloc(*cap);
		if (!new_content)
			return (content);
		i = 0;
		while (i < len)
		{
			new_content[i] = content[i];
			i++;
		}
		content = new_content;
	}
	return (content);
}

void	add_line_to_content(char **content, int *content_len, char *expanded,
		int line_len)
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
	if (sh_strlen(line) == delim_len && sh_strncmp(line, delimiter,
			delim_len) == 0)
	{
		sh_signal_set_state(STATE_HEREDOC, 0);
		return (1);
	}
	return (0);
}

char	*process_heredoc_line_raw(char *content, int *content_len,
		int *content_cap, char *line)
{
	int	line_len;

	line_len = sh_strlen(line);
	content = resize_content(content, content_cap, *content_len, line_len);
	add_line_to_content(&content, content_len, line, line_len);
	return (content);
}
