/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dump.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 22:18:26 by root              #+#    #+#             */
/*   Updated: 2025/09/21 00:06:47 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	dump_gc(void)
{
	t_trash		*trash_node;
	t_trash		*trash_node_swap;
	t_gc		*gc;

	gc = get_gc(GET_GC);
	trash_node = gc->first_node;
	while (trash_node)
	{
		if (trash_node->mem)
			free(trash_node->mem);
		trash_node_swap = trash_node->next;
		free(trash_node);
		trash_node = trash_node_swap;
	}
	free(gc);
}

void	dump_env_gc(void)
{
	t_trash		*trash_node;
	t_trash		*trash_node_swap;
	t_gc		*gc;

	gc = get_env_gc(GET_GC);
	trash_node = gc->first_node;
	while (trash_node)
	{
		if (trash_node->mem)
			free(trash_node->mem);
		trash_node_swap = trash_node->next;
		free(trash_node);
		trash_node = trash_node_swap;
	}
	free(gc);
}