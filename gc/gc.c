/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gc.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:24:47 by root              #+#    #+#             */
/*   Updated: 2025/09/19 12:26:50 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	*gc_exit(void)
{
	dump_gc();
	exit(get_env()->exit_status);
	return (NULL);
}

t_gc	*get_gc(void)
{
	static t_gc	*gc = NULL;

	if (!gc)
		gc = new_gc();
	if (!gc)
		return (gc_exit());
	return (gc);
}

void	insert_to_gc(t_trash *new_trash)
{
	t_gc	*gc;

	gc = get_gc();
	if (!gc->first_node)
	{
		gc->first_node = new_trash;
		gc->gc_mark = gc->first_node;
	}
	else
	{
		(gc->gc_mark)->next = new_trash;
		gc->gc_mark = (gc->gc_mark->next);
	}
}

void	*alloc(ssize_t size)
{
	void	*mem;
	t_trash	*trash;

	mem = malloc(size);
	if (!mem)
		return (gc_exit());
	trash = new_trash(mem);
	if (!trash)
		return (gc_exit());
	insert_to_gc(trash);
	return (mem);
}

void	dump_gc(void)
{
	t_trash	*trash_node;
	t_trash	*trash_node_swap;
	t_gc	*gc;

	trash_node = get_gc()->first_node;
	while (trash_node)
	{
		if (trash_node->mem)
			free(trash_node->mem);
		trash_node_swap = trash_node->next;
		free(trash_node);
		trash_node = trash_node_swap;
	}
	gc = get_gc();
	if (gc)
		free(gc);
}
