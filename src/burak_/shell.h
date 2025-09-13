/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   xshell.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 21:19:29 by btuncer           #+#    #+#             */
/*   Updated: 2025/06/20 19:12:56 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define NAME xshell
#define PROMPT " $shell~ "
#include <unistd.h>
#include <stdbool.h>

typedef struct s_trash_node
{
	void	*mem;
	struct s_trash_node	*next;
}			t_trash;

void *alloc(ssize_t size);
void trash_bin(bool dump, t_trash *trash);