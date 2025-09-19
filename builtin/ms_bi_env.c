/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ms_bi_env.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 17:02:53 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/19 02:39:09 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	print_export_format(void)
{
	char	**sorted_env;
	int		i;

	sorted_env = get_sorted_env();
	if (!sorted_env)
		return ;
	i = 0;
	while (sorted_env[i])
	{
		write(1, "declare -x ", 11);
		write(1, sorted_env[i], len(sorted_env[i]));
		write(1, "\n", 1);
		i++;
	}
}

int	bi_export(char **argv)
{
	int			i;
	char		*eq;
	t_env_item	*item;

	i = 1;
	if (!argv[1])
	{
		print_export_format();
		return (0);
	}
	while (argv[i])
	{
		eq = sh_strchr(argv[i], '=');
		if (!eq)
		{
			item = get_env_item(argv[i]);
			if (!item)
				add_env_item(new_env_item(argv[i], NULL));
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

int	bi_unset(char **argv)
{
	int	i;

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
