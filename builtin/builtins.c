/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:09:27 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/22 02:10:09 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	is_builtin(const char *cmd)
{
	if (!cmd)
		return (0);
	if (sh_strcmp(cmd, "echo") == 0)
		return (1);
	if (sh_strcmp(cmd, "pwd") == 0)
		return (1);
	if (sh_strcmp(cmd, "env") == 0)
		return (1);
	if (sh_strcmp(cmd, "exit") == 0)
		return (1);
	if (sh_strcmp(cmd, "cd") == 0)
		return (1);
	if (sh_strcmp(cmd, "export") == 0)
		return (1);
	if (sh_strcmp(cmd, "unset") == 0)
		return (1);
	return (0);
}

int	run_builtin(char **argv)
{
	if (!argv || !argv[0])
		return (0);
	if (sh_strcmp(argv[0], "echo") == 0)
		return (bi_echo(argv));
	if (sh_strcmp(argv[0], "pwd") == 0)
		return (bi_pwd());
	if (sh_strcmp(argv[0], "env") == 0)
		return (bi_env(argv));
	if (sh_strcmp(argv[0], "exit") == 0)
		bi_exit(argv);
	if (sh_strcmp(argv[0], "cd") == 0)
		return (bi_cd(argv));
	if (sh_strcmp(argv[0], "export") == 0)
		return (bi_export(argv));
	if (sh_strcmp(argv[0], "unset") == 0)
		return (bi_unset(argv));
	return (1);
}
