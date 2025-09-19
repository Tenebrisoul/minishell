/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ms_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 16:52:51 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/19 02:42:02 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	print_echo_string(const char *str)
{
	write(1, str, sh_strlen(str));
}

static int	is_valid_n_flag(const char *arg)
{
	int	i;

	if (!arg || arg[0] != '-' || arg[1] != 'n')
		return (0);
	i = 1;
	while (arg[i])
	{
		if (arg[i] != 'n')
			return (0);
		i++;
	}
	return (i > 1);
}

int	bi_echo(char **argv)
{
	int	i;
	int	newline;
	int	first;

	i = 1;
	newline = 1;
	first = 1;
	while (argv[i] && is_valid_n_flag(argv[i]))
	{
		newline = 0;
		i++;
	}
	while (argv[i])
	{
		if (!first)
			write(1, " ", 1);
		first = 0;
		print_echo_string(argv[i]);
		i++;
	}
	if (newline)
		write(1, "\n", 1);
	return (0);
}
