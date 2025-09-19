/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ms_exit.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 16:59:34 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/19 15:14:40 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	bi_exit(char **argv)
{
	int	code;

	code = get_env()->exit_status;
	if (argv[1])
	{
		code = ft_atol(argv[1]) & 0xFF;
	}
	if (isatty(STDIN_FILENO))
		write(1, "exit\n", 5);
	get_env()->exit_status = code;
	exit(-1);
}
