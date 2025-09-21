/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_print.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:11:49 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/22 02:19:20 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	print_env(void)
{
	t_env_item	*node;

	node = get_env()->first_node;
	while (node)
	{
		if (ft_strcmp(node->key, "__INIT__") || !node->value)
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
