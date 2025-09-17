#include "minishell.h"

void *alloc(ssize_t);

t_env_item *new_env_item(char *key, char *val)
{
    t_env_item *new_item;
    
    new_item = alloc(sizeof(t_env_item));
    new_item->key = alloc(len(key) * sizeof(char) + 1);
    new_item->value = alloc(len(val) * sizeof(char) + 1);
    ft_strcpy(key, new_item->key);
    ft_strcpy(val, new_item->value);
    new_item->next = NULL;
    return (new_item);
}

t_env *new_env()
{
    t_env *new_environment;

    new_environment = alloc(sizeof(t_env));
    if (!new_environment)
        return (NULL);
    new_environment->first_node = NULL;
    new_environment->exit_status = 0;
    // new_environment->display_text = NULL;
    return (new_environment);
}
