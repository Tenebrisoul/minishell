/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_expander.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:12:27 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/22 02:30:45 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_expander	*new_expander(void)
{
	t_expander	*expander;

	expander = alloc(sizeof(t_expander));
	expander->prompt = NULL;
	expander->marker = 0;
	expander->queue = NULL;
	expander->queue_marker = 0;
	return (expander);
}

t_expander	*get_expander(bool create)
{
	static t_expander	*expander = NULL;

	if (!expander || create)
		expander = new_expander();
	return (expander);
}
