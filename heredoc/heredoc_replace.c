/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_replace.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 05:00:00 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/22 05:00:00 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*find_substring(char *str, char *sub)
{
	int	i;
	int	j;

	i = 0;
	while (str[i])
	{
		j = 0;
		while (str[i + j] && sub[j] && str[i + j] == sub[j])
			j++;
		if (!sub[j])
			return (str + i);
		i++;
	}
	return (NULL);
}

static int	copy_before_pos(char *result, char *str, char *pos, int i)
{
	while (str + i < pos)
	{
		result[i] = str[i];
		i++;
	}
	return (i);
}

static int	copy_new_and_after(char *result, char *new, char *pos, int old_len)
{
	int	j;
	int	i;

	i = 0;
	j = 0;
	while (new[j])
		result[i++] = new[j++];
	j = 0;
	while (pos[old_len + j])
		result[i++] = pos[old_len + j++];
	return (i);
}

char	*simple_str_replace(char *str, char *old, char *new)
{
	char	*pos;
	char	*result;
	int		old_len;
	int		total_len;
	int		i;

	pos = find_substring(str, old);
	if (!pos)
		return (str);
	old_len = len(old);
	total_len = len(str) - old_len + len(new) + 1;
	result = alloc(total_len);
	i = copy_before_pos(result, str, pos, 0);
	i += copy_new_and_after(result + i, new, pos, old_len);
	result[i] = '\0';
	return (result);
}

char	*heredoc_replace_all_variables(char *prompt)
{
	t_expander	*expander;
	char		*result;
	char		*expanded_value;
	int			i;

	expander = get_expander(GET);
	result = prompt;
	i = 0;
	while (i < expander->queue_marker && expander->queue[i])
	{
		expanded_value = get_variable_value(expander->queue[i]);
		if (expanded_value)
			result = simple_str_replace(result, expander->queue[i],
					expanded_value);
		else
			result = simple_str_replace(result, expander->queue[i], "");
		i++;
	}
	return (result);
}
