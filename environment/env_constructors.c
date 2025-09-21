/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_constructors.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:24:12 by root              #+#    #+#             */
/*   Updated: 2025/09/20 23:18:22 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static bool	is_key_valid(char *key)
{
	int	i;

	if (!key || !key[0])
		return (false);
	if (!((key[0] >= 'a' && key[0] <= 'z') || (key[0] >= 'A' && key[0] <= 'Z')
			|| key[0] == '_'))
		return (false);
	i = 1;
	while (key[i])
	{
		if (!((key[i] >= 'a' && key[i] <= 'z') || (key[i] >= 'A'
					&& key[i] <= 'Z') || (key[i] >= '0' && key[i] <= '9')
				|| key[i] == '_'))
			return (false);
		i++;
	}
	return (true);
}

t_env_item	*new_env_item(char *key, char *val)
{
	t_env_item	*new_item;

	if (!is_key_valid(key))
		return (NULL);
	new_item = env_alloc(sizeof(t_env_item));
	new_item->key = env_alloc(len(key) * sizeof(char) + 1);
	ft_strcpy(key, new_item->key);
	if (val)
	{
		new_item->value = env_alloc(len(val) * sizeof(char) + 1);
		ft_strcpy(val, new_item->value);
	}
	else
	{
		new_item->value = NULL;
	}
	new_item->next = NULL;
	return (new_item);
}

t_env	*new_env(void)
{
	t_env	*new_environment;

	new_environment = env_alloc(sizeof(t_env));
	if (!new_environment)
		return (NULL);
	new_environment->first_node = NULL;
	new_environment->exit_status = 0;
	return (new_environment);
}
