/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_items.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:11:46 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/22 02:58:41 by btuncer          ###   ########.fr       */
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

	if (ft_strcmp(key, "__INIT__"))
		return ;
	item_exists = is_env_item_exists(key);
	if (!item_exists)
		return ;
	env_item = get_env()->first_node;
	while (!ft_strcmp((env_item->next)->key, key))
		env_item = env_item->next;
	env_item->next = (env_item->next)->next;
}
