/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_env.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:11:10 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/22 02:29:00 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	count_valid_env_items(void)
{
	t_env_item	*node;
	int			count;

	count = 0;
	node = get_env()->first_node;
	while (node)
	{
		if (ft_strcmp(node->key, "__INIT__") == false)
			count++;
		node = node->next;
	}
	return (count);
}

static void	fill_sorted_array(char **sorted_env)
{
	t_env_item	*node;
	int			i;

	i = 0;
	node = get_env()->first_node;
	while (node)
	{
		if (ft_strcmp(node->key, "__INIT__") == false)
			fill_env_item(sorted_env, node, &i);
		node = node->next;
	}
}

char	**get_sorted_env(void)
{
	char	**sorted_env;
	int		count;

	count = count_valid_env_items();
	if (count == 0)
		return (NULL);
	sorted_env = alloc((count + 1) * sizeof(char *));
	if (!sorted_env)
		return (NULL);
	fill_sorted_array(sorted_env);
	bubble_sort_env(sorted_env, count);
	sorted_env[count] = NULL;
	return (sorted_env);
}
