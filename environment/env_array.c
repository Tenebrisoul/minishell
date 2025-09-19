/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_array.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:24:16 by root              #+#    #+#             */
/*   Updated: 2025/09/19 02:24:17 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	count_env_items(void)
{
	t_env_item	*node;
	int			count;

	count = 0;
	node = get_env()->first_node;
	while (node)
	{
		if (!ft_strcmp(node->key, "__INIT__"))
			count++;
		node = node->next;
	}
	return (count);
}

static char	*create_env_string(t_env_item *node)
{
	int		key_len;
	int		val_len;
	char	*env_str;

	key_len = len(node->key);
	val_len = len(node->value);
	env_str = alloc(key_len + val_len + 2);
	if (env_str)
	{
		ft_strcpy(node->key, env_str);
		env_str[key_len] = '=';
		ft_strcpy(node->value, env_str + key_len + 1);
	}
	return (env_str);
}

static void	fill_env_array(char **env_array, int count)
{
	t_env_item	*node;
	int			i;

	i = 0;
	node = get_env()->first_node;
	while (node && i < count)
	{
		if (!ft_strcmp(node->key, "__INIT__"))
			env_array[i++] = create_env_string(node);
		node = node->next;
	}
	env_array[i] = NULL;
}

char	**get_env_array(void)
{
	char	**env_array;
	int		count;

	count = count_env_items();
	env_array = alloc((count + 1) * sizeof(char *));
	if (!env_array)
		return (NULL);
	fill_env_array(env_array, count);
	return (env_array);
}
