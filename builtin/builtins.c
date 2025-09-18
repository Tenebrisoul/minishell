#include "../minishell.h"
#include <unistd.h>
#include <stdlib.h>

static void print_echo_string(const char *str)
{
	write(1, str, sh_strlen(str));
}

static int is_valid_n_flag(const char *arg)
{
	int i;

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

static int bi_echo(char **argv)
{
	int i;
	int newline;
	int first;

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

static int bi_pwd(void)
{
	char buf[4096];
	char *p;

	p = getcwd(buf, sizeof(buf));
	if (!p)
	{
		write(2, "minishell: pwd: error retrieving current directory\n", 51);
		return (1);
	}
	write(1, p, sh_strlen(p));
	write(1, "\n", 1);
	return (0);
}

static int bi_env(char **argv)
{
	(void)argv;
	print_env();
	return (0);
}

static int bi_exit(char **argv)
{
	int code;

	code = get_env()->exit_status;
	if (argv[1])
	{
		// Simple exit code parsing - just use atoi for now
		code = atoi(argv[1]) & 0xFF;
	}
	if (isatty(STDIN_FILENO))
		write(1, "exit\n", 5);
	exit(code);
}

static const char *get_cd_target(char **argv)
{
	const char *target;

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

static int change_directory(const char *target)
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

static void update_pwd_vars(const char *old)
{
	char buf[4096];
	t_env_item *item;

	if (getcwd(buf, sizeof(buf)))
	{
		// Set OLDPWD
		item = get_env_item("OLDPWD");
		if (item)
			item->value = ft_strdup(old);
		else
			add_env_item(new_env_item("OLDPWD", (char *)old));

		// Set PWD
		item = get_env_item("PWD");
		if (item)
			item->value = ft_strdup(buf);
		else
			add_env_item(new_env_item("PWD", buf));
	}
}

static int bi_cd(char **argv)
{
	const char *target;
	char old[4096];

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

static int bi_export(char **argv)
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
		else
			add_env_item(new_env_item(argv[i], eq + 1));
		*eq = '=';
		i++;
	}
	return (0);
}

static int bi_unset(char **argv)
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