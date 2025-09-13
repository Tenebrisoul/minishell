/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_expander.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/12 14:12:04 by root              #+#    #+#             */
/*   Updated: 2025/09/13 14:32:46 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/expand.h"
#include <unistd.h>

t_expander *new_expander()
{
    t_expander *expander;

    expander = alloc(sizeof(t_expander));
    expander->prompt = NULL;
    expander->marker = 0;
    expander->queue = NULL;
    expander->queue_marker = 0;

    return (expander);
}

t_expander *get_expander(bool create)
{
    static t_expander *expander = NULL;

    if (!expander || create)
        expander = new_expander();
    return (expander);
}
