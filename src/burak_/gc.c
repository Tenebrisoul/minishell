/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gc.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 09:41:37 by btuncer           #+#    #+#             */
/*   Updated: 2025/06/26 18:12:10 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./shell.h"
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>

t_trash *create_trash()
{
    t_trash *new_trash;
    
    new_trash = malloc(sizeof(t_trash));
    new_trash->mem = NULL;
    new_trash->next = NULL;
    
    return (new_trash);
}

void trash_bin(bool dump, t_trash *trash)
{
    static t_trash *first_trash = NULL;
    static t_trash *current_trash = NULL;
    t_trash *swap;
    
    if (dump && first_trash)
    {
        current_trash = first_trash;
        while (current_trash)
        {
            if (current_trash->mem)
            free(current_trash->mem);
            swap = current_trash->next;
            free(current_trash);
            current_trash = swap;
        }
        return ;
    }
    if (first_trash == NULL)
    {
        first_trash = trash;
        current_trash = first_trash;
        return ;
    }
    current_trash->next = trash;
    current_trash = current_trash->next;
}

void *alloc(ssize_t size)
{
    void *mem;
    t_trash *new_trash;
    
    mem = malloc(size);
    if (!mem)
    return (NULL);
    new_trash = create_trash();
    new_trash->mem = mem;
    new_trash->next = NULL;
    trash_bin(false, new_trash);
    
    return (mem);
}