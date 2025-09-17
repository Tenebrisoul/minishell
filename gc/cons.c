#include "minishell.h"

t_gc *new_gc()
{
    t_gc *gc;

    gc = malloc(sizeof(t_gc));
    if (!gc)
        return (NULL);
    gc->first_node = NULL;
    gc->gc_mark = NULL;
    return (gc);
}

t_trash *new_trash(void *mem)
{
    t_trash *trash;

    trash = malloc(sizeof(t_trash));
    if (!trash)
        return (NULL);
    trash->mem = mem;
    trash->next = NULL;
    return (trash);
}
