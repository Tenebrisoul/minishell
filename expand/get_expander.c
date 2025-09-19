/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_expander.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:24:39 by root              #+#    #+#             */
/*   Updated: 2025/09/19 02:24:40 by root             ###   ########.fr       */
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
