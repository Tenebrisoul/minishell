/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ms_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 17:00:44 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/19 02:38:34 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static const char	*get_cd_target(char **argv)
{
	const char	*target;

	if (!argv[1])
	{
		target = sh_getenv_val("HOME");
		return (target);
	}
	if (sh_strcmp(argv[1], "-") == 0)
	{
		target = sh_getenv_val("OLDPWD");
		if (!target)
		{
			write(2, "minishell: cd: OLDPWD not set\n", 30);
			return (NULL);
		}
		write(1, target, sh_strlen(target));
		write(1, "\n", 1);
		return (target);
	}
	if (sh_strcmp(argv[1], "~") == 0)
	{
		target = sh_getenv_val("HOME");
		return (target);
	}
	return (argv[1]);
}

static int	change_directory(const char *target)
{
	if (!target)
	{
		write(2, "minishell: cd: HOME not set\n", 28);
		return (1);
	}
	if (chdir(target) != 0)
	{
		write(2, "minishell: cd: ", 15);
		write(2, target, sh_strlen(target));
		write(2, ": No such file or directory\n", 28);
		return (1);
	}
	return (0);
}

static void	update_pwd_vars(const char *old)
{
	char		buf[4096];
	t_env_item	*item;

	if (getcwd(buf, sizeof(buf)))
	{
		item = get_env_item("OLDPWD");
		if (item)
			item->value = ft_strdup(old);
		else
			add_env_item(new_env_item("OLDPWD", (char *)old));
		item = get_env_item("PWD");
		if (item)
			item->value = ft_strdup(buf);
		else
			add_env_item(new_env_item("PWD", buf));
	}
}

int	bi_cd(char **argv)
{
	const char	*target;
	char		old[4096];

	target = get_cd_target(argv);
	if (!target && argv[1] && sh_strcmp(argv[1], "-") == 0)
		return (1);
	if (!getcwd(old, sizeof(old)))
		old[0] = '\0';
	if (change_directory(target))
		return (1);
	update_pwd_vars(old);
	return (0);
}
