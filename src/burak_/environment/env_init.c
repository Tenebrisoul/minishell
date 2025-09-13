/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 19:43:10 by btuncer           #+#    #+#             */
/*   Updated: 2025/06/26 18:10:25 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./env.h"
#include <stdbool.h>
#include <stdlib.h>

char *get_cwd();
t_env_item *is_env_item_exists(char *key);

bool is_ignored_env_item(char *key)
{
    return ((ft_strcmp(key, "PWD") || ft_strcmp(key, "LINES") || ft_strcmp(key, "COLUMNS")
                || ft_strcmp(key, "__INIT__")));
}

void *include_environ(t_env *env)
{
    extern char **environ;
    char **key_val;
    int counter;
    t_env_item *curr_node;

    counter = 0;
    curr_node = env->first_node;
    while (environ[counter])
    {
        key_val = ft_split(environ[counter], '=');
        if (!key_val)
            return (NULL);
        counter++;
        if (is_ignored_env_item(key_val[0]))
            continue;
        curr_node->next = new_env_item(key_val[0], key_val[1]);
        if (!curr_node->next)
            return (NULL);
        curr_node = curr_node->next;
    }
    return (env);
}

t_env *get_env()
{
    static t_env *environment = NULL;

    if (!environment)
    {
        environment = new_env();
        environment->first_node = new_env_item("__INIT__", "INIT");
    }
    if (!environment)
        exit(0);
    return (environment);
}
void unset_env_item(char *key);

void configure_env()
{
    t_env_item *item_exists;
    
    add_env_item(new_env_item("PWD", get_cwd()));
    item_exists = is_env_item_exists("SHLVL");
    if (item_exists)
        item_exists->value = ft_ltoa(ft_atol(item_exists->value) + 1);
    else
        add_env_item(new_env_item("SHLVL", "0"));
}

void *init_env()
{
    t_env *env;

    env = get_env();
    if (!include_environ(env))
        return (NULL);
    configure_env();
    return (env);
}