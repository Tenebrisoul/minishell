/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_print.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:24:16 by root              #+#    #+#             */
/*   Updated: 2025/09/19 02:24:17 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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
