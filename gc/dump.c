/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dump.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 22:18:26 by root              #+#    #+#             */
/*   Updated: 2025/09/20 22:36:57 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	dump_gc(void)
{
	t_trash		*trash_node;
	t_trash		*trash_node_swap;
	t_gc		*gc;
	static int	already_cleaned = 0;

	if (already_cleaned)
		return ;
	already_cleaned = 1;
	gc = get_gc();
	if (!gc)
		return ;
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
	static int	already_cleaned = 0;

	if (already_cleaned)
		return ;
	already_cleaned = 1;
	gc = get_env_gc();
	if (!gc)
		return ;
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