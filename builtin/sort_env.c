/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_env.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 22:35:00 by root              #+#    #+#             */
/*   Updated: 2025/09/19 02:41:48 by root             ###   ########.fr       */
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
	int			key_len;
	int			val_len;

	i = 0;
	node = get_env()->first_node;
	while (node)
	{
		if (ft_strcmp(node->key, "__INIT__") == false)
		{
			key_len = len(node->key);
			val_len = 0;
			if (node->value)
				val_len = len(node->value);
			sorted_env[i] = alloc(key_len + val_len + 4);
			if (sorted_env[i])
			{
				ft_strcpy(node->key, sorted_env[i]);
				if (node->value)
				{
					sorted_env[i][key_len] = '=';
					sorted_env[i][key_len + 1] = '"';
					if (val_len > 0)
						ft_strcpy(node->value, sorted_env[i] + key_len + 2);
					sorted_env[i][key_len + val_len + 2] = '"';
					sorted_env[i][key_len + val_len + 3] = '\0';
				}
				else
				{
					sorted_env[i][key_len] = '\0';
				}
			}
			i++;
		}
		node = node->next;
	}
}

static void	bubble_sort_env(char **env_array, int count)
{
	char	*temp;
	int		i;
	int		j;

	i = 0;
	while (i < count - 1)
	{
		j = 0;
		while (j < count - 1 - i)
		{
			if (sh_strcmp(env_array[j], env_array[j + 1]) > 0)
			{
				temp = env_array[j];
				env_array[j] = env_array[j + 1];
				env_array[j + 1] = temp;
			}
			j++;
		}
		i++;
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
