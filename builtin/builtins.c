/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 16:13:13 by root              #+#    #+#             */
/*   Updated: 2025/09/18 21:50:12 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int bi_true(void)
{
	return (0);
}

static int bi_false(void)
{
	return (1);
}

int is_builtin(const char *cmd)
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
	if (sh_strcmp(cmd, "true") == 0)
		return (1);
	if (sh_strcmp(cmd, "false") == 0)
		return (1);
	return (0);
}

int run_builtin(char **argv)
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
		return (bi_exit(argv));
	if (sh_strcmp(argv[0], "cd") == 0)
		return (bi_cd(argv));
	if (sh_strcmp(argv[0], "export") == 0)
		return (bi_export(argv));
	if (sh_strcmp(argv[0], "unset") == 0)
		return (bi_unset(argv));
	if (sh_strcmp(argv[0], "true") == 0)
		return (bi_true());
	if (sh_strcmp(argv[0], "false") == 0)
		return (bi_false());
	return (1);
}