/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fetch.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:12:25 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/22 01:53:11 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*handle_special_vars(char *key)
{
	if (key[1] == '$')
		return (ft_ltoa(getpid()));
	else if (key[1] == '?')
		return (ft_ltoa(get_env()->exit_status));
	else if (key[1] == '0')
		return (sh_strdup("minishell"));
	return (NULL);
}

static char	*expand_in_dquotes(char *str)
{
	t_env_item	*env_item;

	if (!str || !str[0])
		return (sh_strdup(""));
	if (str[0] == '$' && str[1])
	{
		if (str[1] == '$')
			return (ft_ltoa(getpid()));
		else if (str[1] == '?')
			return (ft_ltoa(get_env()->exit_status));
		else if (str[1] == '0')
			return (sh_strdup("minishell"));
		env_item = get_env_item(str + 1);
		if (env_item && env_item->value)
			return (sh_strdup(env_item->value));
		return (sh_strdup(""));
	}
	return (sh_strdup(str));
}

static char	*handle_quoted_var(char *key)
{
	char	*var_name;
	char	*result;

	var_name = key + 1;
	if (!var_name)
		return (sh_strdup(""));
	if (key[1] == '"')
		result = expand_in_dquotes(var_name);
	else
		result = sh_strdup(var_name);
	return (result);
}

char	*get_variable_value(char *key)
{
	t_env_item	*env_item;

	if (!key || !key[0])
		return (NULL);
	if (key[0] == '$' && key[1] && (key[1] == '$' || key[1] == '?'
			|| key[1] == '0'))
		return (handle_special_vars(key));
	if (key[1] == '\0')
		return (sh_strdup("$"));
	if (key[1] == '"' || key[1] == '\'')
		return (handle_quoted_var(key));
	env_item = get_env_item(key + 1);
	if (env_item && env_item->value)
		return (sh_strdup(env_item->value));
	return (sh_strdup(""));
}
