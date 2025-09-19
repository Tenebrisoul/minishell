/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:24:36 by root              #+#    #+#             */
/*   Updated: 2025/09/19 02:24:37 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	valid_dollar_count(char *str)
{
	int	counter;
	int	valid_dollars;

	counter = 0;
	valid_dollars = 0;
	while (str[counter])
	{
		if (str[counter] == '$')
			if (quote_status(str, counter) == 1)
				valid_dollars++;
		counter++;
	}
	return (valid_dollars);
}

int	expandable_len(char *marked_pos)
{
	int	counter;

	counter = 1;
	if (marked_pos[1] && (marked_pos[1] == '$' || marked_pos[1] == '?'
			|| marked_pos[1] == '0'))
		return (2);
	if (!marked_pos[1] || (!is_char(marked_pos[1]) && marked_pos[1] != '_'))
		return (1);
	counter++;
	while (marked_pos[counter] && (is_char(marked_pos[counter])
			|| is_number(marked_pos[counter]) || marked_pos[counter] == '_'))
		counter++;
	return (counter);
}

void	insert_curr_to_queue(t_expander *expander)
{
	int	new_expandable_size;
	int	marked_pos;
	int	i;

	marked_pos = expander->marker;
	new_expandable_size = expandable_len(expander->prompt + marked_pos);
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

void	queue_expandables(void)
{
	t_expander	*expander;

	expander = get_expander(GET);
	while (expander->prompt[expander->marker])
	{
		if (expander->prompt[expander->marker] == '$'
			&& quote_status(expander->prompt, expander->marker) == 1)
			insert_curr_to_queue(expander);
		expander->marker++;
	}
}
