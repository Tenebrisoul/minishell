#include "../minishell.h"

bool	is_ignored_env_item(char *key)
{
	return ((ft_strcmp(key, "PWD") || ft_strcmp(key, "LINES") || ft_strcmp(key,
				"COLUMNS") || ft_strcmp(key, "__INIT__")));
}

void	*include_environ(t_env *env)
{
	extern char	**environ;
	char		**key_val;
	int			counter;
	t_env_item	*curr_node;

	counter = 0;
	curr_node = env->first_node;
	while (environ[counter])
	{
		key_val = ft_split(environ[counter], '=');
		counter++;
		if (is_ignored_env_item(key_val[0]))
			continue ;
		curr_node->next = new_env_item(key_val[0], key_val[1]);
		curr_node = curr_node->next;
	}
	return (env);
}

t_env	*get_env(void)
{
	static t_env	*environment = NULL;

	if (!environment)
	{
		environment = new_env();
		environment->first_node = new_env_item("__INIT__", "INIT");
	}
	if (!environment)
		gc_exit(1);
	return (environment);
}
void		unset_env_item(char *key);

void	configure_env(void)
{
	t_env_item	*item_exists;

	add_env_item(new_env_item("PWD", get_cwd()));
	item_exists = is_env_item_exists("SHLVL");
	if (item_exists)
		item_exists->value = ft_ltoa_env(ft_atol(item_exists->value) + 1);
	else
		add_env_item(new_env_item("SHLVL", "1"));
}

void	*init_env(void)
{
	t_env	*env;

	env = get_env();
	include_environ(env);
	configure_env();
	return (env);
}
