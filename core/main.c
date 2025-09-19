/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:24:02 by root              #+#    #+#             */
/*   Updated: 2025/09/19 15:14:24 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

__attribute__((constructor)) static void	init_on_start(void)
{
	init_env();
	init_signals();
}

__attribute__((destructor)) static void	cleanup_on_exit(void)
{
	t_env	*env;
	int		exit_status;

	env = get_env();
	if (env)
		exit_status = get_env()->exit_status;
	else
		exit_status = 0;
	dump_gc();
	rl_clear_history();
	exit(exit_status);
}

int	main(void)
{
	shell_run();
	return (get_env()->exit_status);
}
