#include "shell.h"
#include <unistd.h>
#include <stdlib.h>

static void print_echo_string(const char *str)
{
	write(1, str, sh_strlen(str));
}

static int bi_echo(char **argv)
{
	int i;
	int newline;
	int first;

	i = 1;
	newline = 1;
	first = 1;

	if (argv[1] && sh_strcmp(argv[1], "-n") == 0)
	{
		newline = 0;
		i = 2;
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

static int env_echo_command(char **argv)
{
	int i;
	int first;

	i = 2;
	first = 1;
	while (argv[i])
	{
		if (!first)
			write(1, " ", 1);
		first = 0;
		write(1, argv[i], sh_strlen(argv[i]));
		i++;
	}
	write(1, "\n", 1);
	return (0);
}

static int print_environment(t_shell *sh)
{
	int i;

	i = 0;
	while (i < sh->env_len)
	{
		if (sh->env[i])
		{
			write(1, sh->env[i], sh_strlen(sh->env[i]));
			write(1, "\n", 1);
		}
		i++;
	}
	return (0);
}

static int bi_env(t_shell *sh, char **argv)
{
	if (argv[1])
	{
		if (sh_strcmp(argv[1], "echo") == 0)
			return (env_echo_command(argv));
		else
		{
			write(2, "minishell: env: command execution not fully implemented\n", 56);
			return (1);
		}
	}
	else
	{
		return (print_environment(sh));
	}
}

static int parse_exit_sign(const char *arg, int *i)
{
	int sign;

	sign = 1;
	if (arg[0] == '+' || arg[0] == '-')
	{
		if (arg[0] == '-')
			sign = -1;
		(*i)++;
	}
	return (sign);
}

static int parse_exit_digits(const char *arg, int *i, long *val)
{
	int has_digits;

	has_digits = 0;
	while (arg[*i] && arg[*i] >= '0' && arg[*i] <= '9')
	{
		*val = *val * 10 + (arg[*i] - '0');
		(*i)++;
		has_digits = 1;
	}
	return (has_digits);
}

static void exit_with_error(const char *arg)
{
	write(2, "minishell: exit: ", 17);
	write(2, arg, sh_strlen(arg));
	write(2, ": numeric argument required\n", 28);
	if (isatty(STDIN_FILENO))
		write(1, "exit\n", 5);
	exit(2);
}

static int parse_exit_code(const char *arg)
{
	int i;
	int sign;
	long val;
	int has_digits;

	i = 0;
	sign = parse_exit_sign(arg, &i);
	val = 0;
	has_digits = parse_exit_digits(arg, &i, &val);
	if (arg[i] != '\0' || !has_digits)
		exit_with_error(arg);
	return ((int)(sign * val) & 0xFF);
}

static int bi_exit(t_shell *sh, char **argv)
{
	int code;

	code = sh ? sh->last_status : 0;
	if (argv[1])
		code = parse_exit_code(argv[1]);
	if (isatty(STDIN_FILENO))
		write(1, "exit\n", 5);
	exit(code);
}

static const char *get_cd_target(t_shell *sh, char **argv)
{
	const char *target;

	if (!argv[1])
	{
		target = sh_getenv_val(sh, "HOME");
		return (target);
	}
	if (sh_strcmp(argv[1], "-") == 0)
	{
		target = sh_getenv_val(sh, "OLDPWD");
		if (!target)
		{
			write(2, "minishell: cd: OLDPWD not set\n", 30);
			return (NULL);
		}
		write(1, target, sh_strlen(target));
		write(1, "\n", 1);
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

static void update_pwd_vars(t_shell *sh, const char *old)
{
	char buf[4096];

	if (getcwd(buf, sizeof(buf)))
	{
		sh_env_set(sh, "OLDPWD", old, 1);
		sh_env_set(sh, "PWD", buf, 1);
	}
}

static int bi_cd(t_shell *sh, char **argv)
{
	const char *target;
	char old[4096];

	target = get_cd_target(sh, argv);
	if (!target && argv[1] && sh_strcmp(argv[1], "-") == 0)
		return (1);
	if (!getcwd(old, sizeof(old)))
		old[0] = '\0';
	if (change_directory(target))
		return (1);
	update_pwd_vars(sh, old);
	return (0);
}

static int bi_export(t_shell *sh, char **argv)
{
	int i;
	int rc;
	char *eq;

	i = 1;
	rc = 0;
	if (!argv[1])
		return (bi_env(sh, argv));
	while (argv[i])
	{
		eq = sh_strchr(argv[i], '=');
		if (!eq)
		{
			i++;
			continue ;
		}
		*eq = '\0';
		if (sh_env_set(sh, argv[i], eq + 1, 1) != 0)
			rc = 1;
		*eq = '=';
		i++;
	}
	return (rc);
}

static int bi_unset(t_shell *sh, char **argv)
{
	int i;

	i = 1;
	if (!argv[1])
		return (0);
	while (argv[i])
	{
		sh_env_unset(sh, argv[i]);
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

int run_builtin(t_shell *sh, char **argv)
{
	if (!argv || !argv[0])
		return (0);
	if (sh_strcmp(argv[0], "echo") == 0)
		return (bi_echo(argv));
	if (sh_strcmp(argv[0], "pwd") == 0)
		return (bi_pwd());
	if (sh_strcmp(argv[0], "env") == 0)
		return (bi_env(sh, argv));
	if (sh_strcmp(argv[0], "exit") == 0)
		return (bi_exit(sh, argv));
	if (sh_strcmp(argv[0], "cd") == 0)
		return (bi_cd(sh, argv));
	if (sh_strcmp(argv[0], "export") == 0)
		return (bi_export(sh, argv));
	if (sh_strcmp(argv[0], "unset") == 0)
		return (bi_unset(sh, argv));
	if (sh_strcmp(argv[0], "true") == 0)
		return (bi_true());
	if (sh_strcmp(argv[0], "false") == 0)
		return (bi_false());
	return (1);
}
