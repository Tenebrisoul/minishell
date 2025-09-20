/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/14 20:25:27 by root              #+#    #+#             */
/*   Updated: 2025/09/19 20:25:26 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*find_variable_in_string(char *str, char *var_name)
{
	char	*pos;
	int		i;

	pos = str;
	while (*pos)
	{
		if (*pos == '$')
		{
			i = 0;
			while (i < len(var_name) && pos[i] && pos[i] == var_name[i])
				i++;
			if (i == len(var_name))
				return (pos);
		}
		pos++;
	}
	return (NULL);
}

static char	*create_backup(char *pos, int var_len, int *backup_len)
{
	char	*backup;

	if (!pos[var_len])
	{
		*backup_len = 0;
		backup = alloc(1);
		backup[0] = '\0';
	}
	else
	{
		*backup_len = len(pos + var_len);
		backup = alloc((*backup_len + 1) * sizeof(char));
		ft_strcpy(pos + var_len, backup);
	}
	return (backup);
}

void	replace_single_variable(char *str, char *var_name, char *value)
{
	char	*pos;
	char	*backup;
	int		backup_len;
	int		var_len;
	int		val_len;

	pos = find_variable_in_string(str, var_name);
	if (!pos)
		return ;
	var_len = len(var_name);
	val_len = len(value);
	backup = create_backup(pos, var_len, &backup_len);
	ft_strcpy(value, pos);
	if (backup_len > 0)
		ft_strcpy(backup, pos + val_len);
	else
		pos[val_len] = '\0';
}

void	apply_all_replacements(char *result)
{
	t_expander	*expander;
	char		*expanded_value;
	int			i;

	expander = get_expander(GET);
	i = 0;
	while (i < expander->queue_marker && expander->queue[i])
	{
		expanded_value = get_variable_value(expander->queue[i]);
		if (expanded_value)
		{
			replace_single_variable(result, expander->queue[i], expanded_value);
		}
		else
			replace_single_variable(result, expander->queue[i], "\0");
		i++;
	}
}

char	*replace_all_variables(void)
{
	t_expander	*expander;
	char		*result;
	int			final_len;

	expander = get_expander(GET);
	final_len = calculate_final_length();
	if (final_len < len(expander->prompt))
		final_len = len(expander->prompt);
	result = alloc((final_len + 1) * sizeof(char));
	ft_strcpy(expander->prompt, result);
	apply_all_replacements(result);
	return (result);
}
