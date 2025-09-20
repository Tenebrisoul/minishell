/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gc.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:24:47 by root              #+#    #+#             */
/*   Updated: 2025/09/20 22:42:37 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	gc_exit(int code)
{
	get_env()->exit_status = code;
	exit(code);
}

void	insert_to_gc(t_trash *new_trash, int gcd)
{
	t_gc	*gc;

	if (gcd == GC_GC)
		gc = get_gc();
	else if (gcd == ENV_GC)
		gc = get_env_gc();

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
	{
		gc_exit(1);
		return (NULL);
	}
	trash = new_trash(mem);
	if (!trash)
	{
		gc_exit(1);
		return (NULL);
	}
	insert_to_gc(trash, GC_GC);
	return (mem);
}

void	*env_alloc(ssize_t size)
{
	void	*mem;
	t_trash	*trash;

	mem = malloc(size);
	if (!mem)
	{
		gc_exit(1);
		return (NULL);
	}
	trash = new_trash(mem);
	if (!trash)
	{
		gc_exit(1);
		return (NULL);
	}
	insert_to_gc(trash, ENV_GC);
	return (mem);
}


