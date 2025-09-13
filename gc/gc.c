/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gc.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell <minishell@student.42>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 00:00:00 by minishell         #+#    #+#             */
/*   Updated: 2025/09/11 00:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gc.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

static t_gc g_gc = {NULL, 0, 0};

static t_gc_node *create_gc_node(void *ptr, size_t size)
{
	t_gc_node *node = malloc(sizeof(t_gc_node));
	if (!node)
		return (NULL);
	node->ptr = ptr;
	node->size = size;
	node->marked = false;
	node->next = NULL;
	return (node);
}

static void add_to_gc(void *ptr, size_t size)
{
	t_gc_node *node = create_gc_node(ptr, size);
	if (!node)
		return;
	node->next = g_gc.head;
	g_gc.head = node;
	g_gc.total_allocated += size;
	g_gc.allocation_count++;
}

static void remove_from_gc(void *ptr)
{
	t_gc_node *current = g_gc.head;
	t_gc_node *prev = NULL;
	
	while (current)
	{
		if (current->ptr == ptr)
		{
			if (prev)
				prev->next = current->next;
			else
				g_gc.head = current->next;
			g_gc.total_allocated -= current->size;
			g_gc.allocation_count--;
			free(current);
			return;
		}
		prev = current;
		current = current->next;
	}
}

void *gc_malloc(size_t size)
{
	void *ptr = malloc(size);
	if (!ptr)
		return (NULL);
	add_to_gc(ptr, size);
	return (ptr);
}

void *gc_realloc(void *old_ptr, size_t new_size)
{
	if (!old_ptr)
		return (gc_malloc(new_size));
	
	remove_from_gc(old_ptr);
	void *new_ptr = realloc(old_ptr, new_size);
	if (!new_ptr)
	{
		add_to_gc(old_ptr, 0);
		return (NULL);
	}
	add_to_gc(new_ptr, new_size);
	return (new_ptr);
}

char *gc_strdup(const char *s)
{
	if (!s)
		return (NULL);
	size_t len = strlen(s);
	char *dup = gc_malloc(len + 1);
	if (!dup)
		return (NULL);
	strcpy(dup, s);
	return (dup);
}

void gc_init(void)
{
	g_gc.head = NULL;
	g_gc.total_allocated = 0;
	g_gc.allocation_count = 0;
}

void gc_cleanup(void)
{
	t_gc_node *current = g_gc.head;
	t_gc_node *next;
	
	while (current)
	{
		next = current->next;
		free(current->ptr);
		free(current);
		current = next;
	}
	gc_init();
}

void gc_mark_ptr(void *ptr)
{
	t_gc_node *current = g_gc.head;
	
	while (current)
	{
		if (current->ptr == ptr)
		{
			current->marked = true;
			return;
		}
		current = current->next;
	}
}

void gc_unmark_all(void)
{
	t_gc_node *current = g_gc.head;
	
	while (current)
	{
		current->marked = false;
		current = current->next;
	}
}

void gc_collect(void)
{
	t_gc_node *current = g_gc.head;
	t_gc_node *prev = NULL;
	t_gc_node *next;
	
	while (current)
	{
		next = current->next;
		if (!current->marked)
		{
			if (prev)
				prev->next = next;
			else
				g_gc.head = next;
			g_gc.total_allocated -= current->size;
			g_gc.allocation_count--;
			free(current->ptr);
			free(current);
		}
		else
		{
			prev = current;
		}
		current = next;
	}
}

size_t gc_get_memory_usage(void)
{
	return (g_gc.total_allocated);
}

void gc_print_stats(void)
{
	char buf[256];
	int len = snprintf(buf, sizeof(buf), 
		"[GC] Allocations: %zu, Total Memory: %zu bytes\n",
		g_gc.allocation_count, g_gc.total_allocated);
	write(2, buf, len);
}
