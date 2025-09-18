/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ms_bi_env.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 17:02:53 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/18 18:48:04 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include "builtins.h"

int bi_export(char **argv)
{
	int i;
	char *eq;
	t_env_item *item;

	i = 1;
	if (!argv[1])
		return (bi_env(argv));
	while (argv[i])
	{
		eq = sh_strchr(argv[i], '=');
		if (!eq)
		{
			i++;
			continue ;
		}
		*eq = '\0';
		item = get_env_item(argv[i]);
		if (item)
			item->value = ft_strdup(eq + 1);
		else if (add_env_item(new_env_item(argv[i], eq + 1)) == NULL)
		{
			write(2, "minishell: not a valid identifier\n", 34);
			return (1);
		}	
		*eq = '=';
		i++;
	}
	return (0);
}

int bi_unset(char **argv)
{
	int i;

	i = 1;
	if (!argv[1])
		return (0);
	while (argv[i])
	{
		unset_env_item(argv[i]);
		i++;
	}
	return (0);
}