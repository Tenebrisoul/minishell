/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:24:16 by root              #+#    #+#             */
/*   Updated: 2025/09/19 02:24:17 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_env_item	*is_env_item_exists(char *key)
{
	t_env_item	*node;

	node = get_env()->first_node;
	while (node)
	{
		if (ft_strcmp(node->key, key))
			return (node);
		node = node->next;
	}
	return (NULL);
}

t_env_item	*add_env_item(t_env_item *new_item)
{
	t_env_item	*node;
	t_env_item	*node_exists;

	if (!new_item)
		return (NULL);
	node = get_env()->first_node;
	node_exists = is_env_item_exists(new_item->key);
	if (node_exists)
	{
		node_exists->value = new_item->value;
		return (new_item);
	}
	while (node->next)
		node = node->next;
	node->next = new_item;
	return (node);
}

t_env_item	*get_env_item(char *key)
{
	t_env_item	*item_exists;

	item_exists = is_env_item_exists(key);
	if (!item_exists)
		return (NULL);
	return (item_exists);
}

void	unset_env_item(char *key)
{
	t_env_item	*item_exists;
	t_env_item	*env_item;

	item_exists = is_env_item_exists(key);
	if (!item_exists)
		return ;
	env_item = get_env()->first_node;
	while (!ft_strcmp((env_item->next)->key, key))
		env_item = env_item->next;
	env_item->next = (env_item->next)->next;
}

void	print_env(void)
{
	t_env_item	*node;

	node = get_env()->first_node;
	while (node)
	{
		if (ft_strcmp(node->key, "__INIT__"))
		{
			node = node->next;
			continue ;
		}
		write(1, node->key, len(node->key));
		write(1, "=", 1);
		write(1, node->value, len(node->value));
		write(1, "\n", 1);
		node = node->next;
	}
}

char	**get_env_array(void)
{
	t_env_item	*node;
	char		**env_array;
	int			count;
	int			i;
	int			key_len;
	int			val_len;
	char		*env_str;

	count = 0;
	node = get_env()->first_node;
	while (node)
	{
		if (!ft_strcmp(node->key, "__INIT__"))
			count++;
		node = node->next;
	}
	env_array = alloc((count + 1) * sizeof(char *));
	if (!env_array)
		return (NULL);
	i = 0;
	node = get_env()->first_node;
	while (node && i < count)
	{
		if (!ft_strcmp(node->key, "__INIT__"))
		{
			key_len = len(node->key);
			val_len = len(node->value);
			env_str = alloc(key_len + val_len + 2);
			if (env_str)
			{
				ft_strcpy(node->key, env_str);
				env_str[key_len] = '=';
				ft_strcpy(node->value, env_str + key_len + 1);
				env_array[i++] = env_str;
			}
		}
		node = node->next;
	}
	env_array[i] = NULL;
	return (env_array);
}
