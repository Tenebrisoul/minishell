/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:24:02 by root              #+#    #+#             */
/*   Updated: 2025/09/19 12:50:36 by root             ###   ########.fr       */
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
	gc_exit();
}

int	main(void)
{
	shell_run();
	return (get_env()->exit_status);
}
