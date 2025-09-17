#include "minishell.h"

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
        return ;
    var_len = len(var_name);
    val_len = len(value);
    backup_len = len(pos + var_len);
    backup = alloc((backup_len + 1) * sizeof(char));
    ft_strcpy(pos + var_len, backup);
    ft_strcpy(value, pos);
    ft_strcpy(backup, pos + val_len);
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
    ft_strcpy(expander->prompt, result);
    apply_all_replacements(result);
    return (result);
}
