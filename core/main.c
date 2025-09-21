/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:11:27 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/22 02:36:43 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include <readline/readline.h>
#include <stdlib.h>

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
	get_gc(RESET_GC);
	dump_env_gc();
	get_env_gc(RESET_GC);
	rl_clear_history();
	exit(exit_status);
}

int	main(void)
{
	shell_run();
	return (get_env()->exit_status);
}
