#include "minishell.h"

char *get_variable_value(char *key)
{
    t_env_item *env_item;

    if (!key || !key[0])
        return (NULL);
    if (key[0] == '$' && key[1] && (key[1] == '$' || key[1] == '?' || key[1] == '0'))
    {
        if (key[1] == '$')
            return (ft_ltoa(getpid()));
        else if (key[1] == '?')
            return (ft_ltoa(get_env()->exit_status));
        else if (key[1] == '0')
            return (ft_strdup("minishell"));
    }
    if (key[1] == '\0')
        return (ft_strdup("$"));
    env_item = get_env_item(key + 1);
    if (env_item && env_item->value)
        return (ft_strdup(env_item->value));
    return (ft_strdup(""));
}