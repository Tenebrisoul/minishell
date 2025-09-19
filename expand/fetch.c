/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fetch.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/14 20:24:11 by root              #+#    #+#             */
/*   Updated: 2025/09/19 14:11:16 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*get_variable_value(char *key)
{
	t_env_item	*env_item;

	if (!key || !key[0])
		return (NULL);
	if (key[0] == '$' && key[1] && (key[1] == '$' || key[1] == '?'
			|| key[1] == '0'))
	{
		if (key[1] == '$')
			return (ft_ltoa(getpid())); // note
		else if (key[1] == '?')
			return (ft_ltoa(get_env()->exit_status));
		else if (key[1] == '0')
			return (sh_strdup("minishell"));
	}
	if (key[1] == '\0')
		return (sh_strdup("$"));
	env_item = get_env_item(key + 1);
	if (env_item && env_item->value)
		return (sh_strdup(env_item->value));
	return (sh_strdup(""));
}
