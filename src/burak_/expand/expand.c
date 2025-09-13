/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 21:30:23 by root              #+#    #+#             */
/*   Updated: 2025/09/13 22:36:08 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <readline/readline.h>
#include <readline/history.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/expand.h"
#include "../environment/env.h"
#include "../libft/libft.h"
#include "../shell.h"

void *init_env();

int valid_dollar_count(char *str)
{
    int counter;
    int valid_dollars;
    
    counter = 0;
    valid_dollars = 0;
    while (str[counter])
    {
        if (str[counter] == '$')
            if (quote_status(str, counter) == 1)
                valid_dollars++;
        counter++;
    }
    return (valid_dollars);
}

char *get_variable_value(char *key)
{
    t_env_item *env_item;

    if (!key || !key[0])
        return (NULL);
    if (key[0] == '$' && key[1] && (key[1] == '$' || key[1] == '?' || key[1] == '0'))
    {
        if (key[1] == '$')
            return (ft_ltoa(getpid())); // note
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

int calculate_result_size()
{
    t_expander *expander;
    char *expanded_value;
    int result_len;
    int i;

    expander = get_expander(GET);
    result_len = len(expander->prompt);
    i = 0;
    while (expander->queue[i])
    {
        expanded_value = get_variable_value(expander->queue[i]);
        if (expanded_value)
        {
            result_len += len(expanded_value);
            free(expanded_value);
        }
        i++;
    }
    return (result_len + 100);
}

char *find_variable_in_string(char *str, char *var_name)
{
    char *pos;
    int i;

    pos = str;
    while (*pos)
    {
        if (*pos == '$')
        {
            i = 0;
            while (i < len(var_name) && pos[i] == var_name[i])
                i++;
            if (i == len(var_name))
                return (pos);
        }
        pos++;
    }
    return (NULL);
}

void replace_single_variable(char *str, char *var_name, char *value)
{
    char *pos;
    char *backup;
    int backup_len;
    int var_len;
    int val_len;

    pos = find_variable_in_string(str, var_name);
    if (!pos)
        return;

    var_len = len(var_name);
    val_len = len(value);
    backup_len = len(pos + var_len);

    backup = alloc((backup_len + 1) * sizeof(char));
    if (!backup)
        return;

    ft_strcpy(pos + var_len, backup);
    ft_strcpy(value, pos);
    ft_strcpy(backup, pos + val_len);

    free(backup);
}

int calculate_final_length()
{
    t_expander *expander;
    char *expanded_value;
    int final_len;
    int i;

    expander = get_expander(GET);
    final_len = len(expander->prompt);
    i = 0;
    while (i < expander->queue_marker && expander->queue[i])
    {
        expanded_value = get_variable_value(expander->queue[i]);
        if (expanded_value)
        {
            final_len = final_len - len(expander->queue[i]) + len(expanded_value);
            free(expanded_value);
        }
        i++;
    }
    return (final_len);
}

void apply_all_replacements(char *result)
{
    t_expander *expander;
    char *expanded_value;
    int i;

    expander = get_expander(GET);
    i = 0;
    while (i < expander->queue_marker && expander->queue[i])
    {
        expanded_value = get_variable_value(expander->queue[i]);
        if (expanded_value)
        {
            replace_single_variable(result, expander->queue[i], expanded_value);
            free(expanded_value);
        }
        i++;
    }
}

char *replace_all_variables()
{
    t_expander *expander;
    char *result;
    int final_len;

    expander = get_expander(GET);
    final_len = calculate_final_length();
    result = alloc((final_len + 1) * sizeof(char));
    if (!result)
        return (NULL);
    ft_strcpy(expander->prompt, result);
    apply_all_replacements(result);
    return (result);
}

char *remove_outer_quotes(char *str)
{
    char *result;
    int i, j;
    char quote_char;

    result = alloc((len(str) + 1) * sizeof(char));
    if (!result)
        return (NULL);
    i = 0;
    j = 0;
    while (str[i])
    {
        if ((str[i] == '\'' || str[i] == '\"'))
        {
            quote_char = str[i];
            i++;
            while (str[i] && str[i] != quote_char)
                result[j++] = str[i++];
            if (str[i] == quote_char)
                i++;
        }
        else
            result[j++] = str[i++];
    }
    result[j] = '\0';
    return (result);
}

void expand(char *prompt)
{
    t_expander *expander;
    char *expanded_prompt;
    char *final_prompt;

    expander = get_expander(INIT);
    expander->prompt = prompt;
    expander->queue = alloc((valid_dollar_count(expander->prompt) + 1) * sizeof(char *));
    expander->queue[valid_dollar_count(expander->prompt)] = NULL;
    queue_expandables();
    expanded_prompt = replace_all_variables();
    if (expanded_prompt)
    {
        final_prompt = remove_outer_quotes(expanded_prompt);
        if (final_prompt)
        {
            printf("%s\n", final_prompt);
            free(final_prompt);
        }
        free(expanded_prompt);
    }
}

int main()
{
    init_env();
    using_history();
    while (1) {
        char *str = readline("smash or pass: ");
        if (!str)
            break;
        add_history(str);
        expand(str);
        free(str);
    }
}
