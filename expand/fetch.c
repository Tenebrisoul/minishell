#include "../minishell.h"

static char	*handle_special_vars(char *key)
{
	if (key[1] == '$')
		return (ft_ltoa(getpid()));
	else if (key[1] == '?')
		return (ft_ltoa(get_env()->exit_status));
	else if (key[1] == '0')
		return (sh_strdup("minishell"));
	return (NULL);
}

static char	*handle_quoted_var(char *key)
{
	char	*var_name;
	char	*result;

	var_name = remove_outer_quotes(key + 1);
	if (!var_name)
		return (sh_strdup(""));
	result = sh_strdup(var_name);
	return (result);
}

char	*get_variable_value(char *key)
{
	t_env_item	*env_item;

	if (!key || !key[0])
		return (NULL);
	if (key[0] == '$' && key[1] && (key[1] == '$' || key[1] == '?'
			|| key[1] == '0'))
		return (handle_special_vars(key));
	if (key[1] == '\0')
		return (sh_strdup("$"));
	if (key[1] == '"' || key[1] == '\'')
		return (handle_quoted_var(key));
	env_item = get_env_item(key + 1);
	if (env_item && env_item->value)
		return (sh_strdup(env_item->value));
	return (sh_strdup(""));
}
