/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cons.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:12:43 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/21 23:12:44 by btuncer          ###   ########.fr       */
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
