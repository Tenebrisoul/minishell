#include "../minishell.h"

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
