/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_main.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 21:30:23 by root              #+#    #+#             */
/*   Updated: 2025/09/19 14:15:38 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	*init_env(void);

int	calculate_result_size(void)
{
	t_expander	*expander;
	char		*expanded_value;
	int			result_len;
	int			i;

	expander = get_expander(GET);
	result_len = len(expander->prompt);
	i = 0;
	while (expander->queue[i])
	{
		expanded_value = get_variable_value(expander->queue[i]);
		if (expanded_value)
		{
			result_len += len(expanded_value);
		}
		i++;
	}
	return (result_len + 100);
}

int	calculate_final_length(void)
{
	t_expander	*expander;
	char		*expanded_value;
	int			final_len;
	int			i;

	expander = get_expander(GET);
	final_len = len(expander->prompt);
	i = 0;
	while (i < expander->queue_marker && expander->queue[i])
	{
		expanded_value = get_variable_value(expander->queue[i]);
		if (expanded_value)
		{
			final_len = final_len - len(expander->queue[i])
				+ len(expanded_value);
		}
		else
			final_len = final_len - len(expander->queue[i]);
		i++;
	}
	return (final_len);
}

char	*expand(char *prompt)
{
	t_expander	*expander;
	char		*expanded_prompt;
	char		*final_prompt;

	expander = get_expander(INIT);
	expander->prompt = prompt;
	expander->queue = alloc((valid_dollar_count(expander->prompt) + 1)
			* sizeof(char *));
	expander->queue[valid_dollar_count(expander->prompt)] = NULL;
	queue_expandables();
	expanded_prompt = replace_all_variables();
	if (expanded_prompt)
	{
		final_prompt = remove_outer_quotes(expanded_prompt);
		if (final_prompt)
		{
			return (final_prompt);
		}
	}
	return (NULL);
}
