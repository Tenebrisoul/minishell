/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gc.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell <minishell@student.42>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 00:00:00 by minishell         #+#    #+#             */
/*   Updated: 2025/09/11 00:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GC_H
# define GC_H

# include <stddef.h>
# include <stdbool.h>

typedef struct s_gc_node
{
	void				*ptr;
	size_t				size;
	bool				marked;
	struct s_gc_node	*next;
}	t_gc_node;

typedef struct s_gc
{
	t_gc_node	*head;
	size_t		total_allocated;
	size_t		allocation_count;
}	t_gc;

void	*gc_malloc(size_t size);
void	*gc_realloc(void *ptr, size_t new_size);
char	*gc_strdup(const char *s);
void	gc_init(void);
void	gc_cleanup(void);
void	gc_collect(void);
void	gc_mark_ptr(void *ptr);
void	gc_unmark_all(void);
size_t	gc_get_memory_usage(void);
void	gc_print_stats(void);

#endif
