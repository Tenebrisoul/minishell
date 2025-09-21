/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cons.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:24:45 by root              #+#    #+#             */
/*   Updated: 2025/09/21 00:05:47 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_gc	*new_gc(void)
{
	t_gc	*gc;

	gc = malloc(sizeof(t_gc));
	if (!gc)
		return (NULL);
	gc->first_node = NULL;
	gc->gc_mark = NULL;
	return (gc);
}

t_trash	*new_trash(void *mem)
{
	t_trash	*trash;

	trash = malloc(sizeof(t_trash));
	if (!trash)
		return (NULL);
	trash->mem = mem;
	trash->next = NULL;
	return (trash);
}

t_gc	*get_gc(int option)
{
	static t_gc	*gc = NULL;

	if (option == RESET_GC)
	{
		gc = NULL;
		return (gc);
	}
	if (!gc)
		gc = new_gc();
	if (!gc)
		gc_exit(1);
	return (gc);
}

t_gc	*get_env_gc(int option)
{
	static t_gc	*env_gc = NULL;

	if (option == RESET_GC)
	{
		env_gc = NULL;
		return (env_gc);
	}
	if (!env_gc)
		env_gc = new_gc();
	if (!env_gc)
		gc_exit(1);
	return (env_gc);
}