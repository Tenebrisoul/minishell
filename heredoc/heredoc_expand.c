/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_expand.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:13:43 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/22 05:00:00 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	heredoc_valid_dollar_count(char *str)
{
	int	counter;
	int	valid_dollars;

	counter = 0;
	valid_dollars = 0;
	while (str[counter])
	{
		if (str[counter] == '$' && !is_escaped(str, counter))
			valid_dollars++;
		counter++;
	}
	return (valid_dollars);
}

int	heredoc_expandable_len(char *marked_pos)
{
	int	counter;

	counter = 1;
	if (!marked_pos[1] || (!is_char(marked_pos[1]) && marked_pos[1] != '_'))
		return (1);
	counter++;
	while (marked_pos[counter] && (is_char(marked_pos[counter])
			|| is_number(marked_pos[counter]) || marked_pos[counter] == '_'))
		counter++;
	return (counter);
}

void	heredoc_insert_curr_to_queue(t_expander *expander)
{
	int	new_expandable_size;
	int	marked_pos;
	int	i;

	marked_pos = expander->marker;
	if ((expander->prompt + marked_pos)[1] && ((expander->prompt
				+ marked_pos)[1] == '$' || (expander->prompt
				+ marked_pos)[1] == '?' || (expander->prompt
				+ marked_pos)[1] == '0'))
		new_expandable_size = 2;
	else
		new_expandable_size = heredoc_expandable_len(expander->prompt
				+ marked_pos);
	expander->queue[expander->queue_marker] = alloc(new_expandable_size + 1);
	i = 0;
	while (i < new_expandable_size && expander->prompt[marked_pos + i])
	{
		expander->queue[expander->queue_marker][i] = expander->prompt[marked_pos
			+ i];
		i++;
	}
	expander->queue[expander->queue_marker][i] = '\0';
	expander->marker = marked_pos + new_expandable_size - 1;
	expander->queue_marker++;
}

static char	*expand_heredoc(char *prompt)
{
	t_expander	*expander;
	char		*expanded_prompt;
	char		*final_prompt;
	char		*processed_prompt;

	expander = get_expander(INIT);
	expander->prompt = prompt;
	expander->queue = alloc((heredoc_valid_dollar_count(expander->prompt) + 1)
			* sizeof(char *));
	expander->queue[heredoc_valid_dollar_count(expander->prompt)] = NULL;
	heredoc_queue_expandables();
	expanded_prompt = heredoc_replace_all_variables(prompt);
	if (expanded_prompt)
	{
		final_prompt = expanded_prompt;
		if (final_prompt)
		{
			processed_prompt = process_escapes(final_prompt);
			if (processed_prompt)
				return (processed_prompt);
			return (final_prompt);
		}
	}
	return (NULL);
}

char	*expand_heredoc_content(char *content, int quoted)
{
	char	*expanded_content;

	if (quoted)
		return (content);
	expanded_content = expand_heredoc(content);
	if (!expanded_content)
		return (content);
	return (expanded_content);
}
