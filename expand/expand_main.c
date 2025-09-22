/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_main.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:10:31 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/22 02:30:20 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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
		if (!expander->queue[i] || !expander->queue[i][0])
		{
			i++;
			continue ;
		}
		expanded_value = get_variable_value(expander->queue[i]);
		if (expanded_value)
			final_len = final_len - len(expander->queue[i])
				+ len(expanded_value);
		else
			final_len = final_len - len(expander->queue[i]);
		i++;
	}
	return (++final_len);
}

static void	handle_escape_char(char *str, char *result, int *i, int *j)
{
	if (str[*i] == '\\' && str[*i + 1])
	{
		if (str[*i + 1] == '$' || str[*i + 1] == '\\')
		{
			result[(*j)++] = str[*i + 1];
			*i += 2;
		}
		else
			result[(*j)++] = str[(*i)++];
	}
	else
		result[(*j)++] = str[(*i)++];
}

char	*process_escapes(char *str)
{
	char	*result;
	int		i;
	int		j;

	result = alloc((len(str) + 1) * sizeof(char));
	if (!result)
		return (NULL);
	i = 0;
	j = 0;
	while (str[i])
		handle_escape_char(str, result, &i, &j);
	result[j] = '\0';
	return (result);
}

static void	renew_prompt(void)
{
	char		*new_prompt;
	t_expander	*expander;
	char		*prompt;
	int			new_size;
	int			prompt_len;

	expander = get_expander(GET);
	prompt = expander->prompt;
	prompt_len = len(prompt);
	new_size = prompt_len - calc_total_expandables() + calc_total_expansions()
		+ 1;
	if (new_size < prompt_len + calc_total_expansions())
		new_size = prompt_len + calc_total_expansions() + 1;
	if (new_size < prompt_len + 16)
		new_size = prompt_len + 16;
	new_prompt = alloc(new_size);
	ft_strcpy(prompt, new_prompt);
	expander->prompt = new_prompt;
}

char	*expand(char *prompt)
{
	t_expander	*expander;
	char		*expanded_prompt;
	char		*final_prompt;
	char		*processed_prompt;

	expander = get_expander(INIT);
	expander->prompt = prompt;
	expander->queue = alloc((valid_dollar_count(expander->prompt) + 1)
			* sizeof(char *));
	expander->queue[valid_dollar_count(expander->prompt)] = NULL;
	queue_expandables();
	renew_prompt();
	expanded_prompt = replace_all_variables();
	if (expanded_prompt)
	{
		final_prompt = remove_outer_quotes(expanded_prompt);
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
