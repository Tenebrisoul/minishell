#include "../minishell.h"

static void	print_export_format(void)
{
	char	**sorted_env;
	int		i;

	sorted_env = get_sorted_env();
	if (!sorted_env)
		return ;
	i = 0;
	while (sorted_env[i])
	{
		write(1, "declare -x ", 11);
		write(1, sorted_env[i], len(sorted_env[i]));
		write(1, "\n", 1);
		i++;
	}
}

static int	export_without_value(char *arg)
{
	t_env_item	*item;

	item = get_env_item(arg);
	if (!item)
	{
		if (add_env_item(new_env_item(arg, NULL)) == NULL)
		{
			write(2, "minishell: not a valid identifier\n", 34);
			return (1);
		}
	}
	return (0);
}

static int	export_with_value(char *arg, char *eq)
{
	t_env_item	*item;

	*eq = '\0';
	item = get_env_item(arg);
	if (item)
		item->value = sh_env_strdup(eq + 1);
	else if (add_env_item(new_env_item(arg, eq + 1)) == NULL)
	{
		write(2, "minishell: not a valid identifier\n", 34);
		*eq = '=';
		return (1);
	}
	*eq = '=';
	return (0);
}

int	bi_export(char **argv)
{
	int		i;
	char	*eq;

	if (!argv[1])
	{
		print_export_format();
		return (0);
	}
	i = 1;
	while (argv[i])
	{
		eq = sh_strchr(argv[i], '=');
		if (!eq)
			return (export_without_value(argv[i]));
		else if (export_with_value(argv[i], eq))
			return (1);
		i++;
	}
	return (0);
}

int	bi_unset(char **argv)
{
	int	i;

	i = 1;
	if (!argv[1])
		return (0);
	if (argv[1][0] == '-')
	{
		write(2, "minishell: unset: invalid option\n", 33);
		return (2);
	}
	while (argv[i])
	{
		unset_env_item(argv[i]);
		i++;
	}
	return (0);
}
