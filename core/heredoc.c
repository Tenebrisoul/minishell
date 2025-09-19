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

static int	is_quoted_delimiter(const char *delimiter, char **clean_delimiter)
{
	int	len;

	len = sh_strlen(delimiter);
	if (len >= 2 && ((delimiter[0] == '"' && delimiter[len - 1] == '"')
			|| (delimiter[0] == '\'' && delimiter[len - 1] == '\'')))
	{
		*clean_delimiter = malloc(len - 1);
		if (!*clean_delimiter)
			return (0);
		strncpy(*clean_delimiter, delimiter + 1, len - 2);
		(*clean_delimiter)[len - 2] = '\0';
		return (1);
	}
	*clean_delimiter = (char *)delimiter;
	return (0);
}

static void	cleanup_heredoc_state(int quoted, char *clean_delimiter,
					const char *delimiter)
{
	sh_signal_set_state(STATE_HEREDOC, 0);
	sh_signal_reset();
	if (quoted && clean_delimiter != delimiter)
		free(clean_delimiter);
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
		*content = process_heredoc_line_raw(*content, content_len,
				content_cap, line);
		if (!*content)
			return (-1);
	}
	return (0);
}

static char	*handle_heredoc_error(int quoted, char *clean_delimiter,
					const char *delimiter)
{
	char	*content;

	cleanup_heredoc_state(quoted, clean_delimiter, delimiter);
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
		return (handle_heredoc_error(quoted, clean_delimiter, delimiter));
	sh_signal_set_state(STATE_HEREDOC, 0);
	if (quoted && clean_delimiter != delimiter)
		free(clean_delimiter);
	if (sh_signal_interrupted())
		return (handle_heredoc_error(0, NULL, NULL));
	content = expand_heredoc_content(content, quoted);
	return (content);
}
