/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:11:22 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/21 23:11:23 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	is_quoted_delimiter(const char *delimiter, char **clean_delimiter)
{
	if (delimiter[0] == '$' && (delimiter[1] == '"' || delimiter[1] == '\''))
	{
		*clean_delimiter = remove_outer_quotes((char *)(delimiter + 1));
		if (!*clean_delimiter)
		{
			*clean_delimiter = (char *)delimiter;
			return (0);
		}
		return (0);
	}
	if (has_quotes(delimiter))
	{
		*clean_delimiter = remove_outer_quotes((char *)delimiter);
		if (!*clean_delimiter)
			return (0);
		return (1);
	}
	*clean_delimiter = (char *)delimiter;
	return (0);
}

static void	cleanup_heredoc_state(void)
{
	sh_signal_set_state(STATE_HEREDOC, 0);
	sh_signal_set_state(STATE_INTERRUPT, 0);
	sh_signal_set_state(STATE_COMMAND, 0);
}

static int	process_heredoc_loop(char **content, int *content_len,
		int *content_cap, char *clean_delimiter)
{
	char	*line;
	int		delimiter_len;

	delimiter_len = sh_strlen(clean_delimiter);
	while (1)
	{
		if (sh_signal_interrupted())
			return (-1);
		line = get_heredoc_line();
		if (!line)
		{
			if (sh_signal_interrupted())
				return (-1);
			print_heredoc_warning(clean_delimiter);
			break ;
		}
		if (check_delimiter(line, clean_delimiter, delimiter_len))
			break ;
		*content = process_heredoc_line_raw(*content, content_len, content_cap,
				line);
		if (!*content)
			return (-1);
	}
	return (0);
}

static char	*handle_heredoc_error(void)
{
	char	*content;

	cleanup_heredoc_state();
	get_env()->exit_status = 130;
	content = (char *)alloc(1);
	if (content)
		content[0] = '\0';
	return (content);
}

char	*read_heredoc_input(const char *delimiter)
{
	char	*content;
	int		content_len;
	int		content_cap;
	char	*clean_delimiter;
	int		quoted;

	quoted = is_quoted_delimiter(delimiter, &clean_delimiter);
	content = (char *)alloc(1024);
	if (!content)
		return (NULL);
	content[0] = '\0';
	content_len = 0;
	content_cap = 1024;
	sh_signal_set_state(STATE_HEREDOC, 1);
	if (process_heredoc_loop(&content, &content_len, &content_cap,
			clean_delimiter) == -1)
		return (handle_heredoc_error());
	sh_signal_set_state(STATE_HEREDOC, 0);
	if (sh_signal_interrupted())
		return (handle_heredoc_error());
	content = expand_heredoc_content(content, quoted);
	return (content);
}
