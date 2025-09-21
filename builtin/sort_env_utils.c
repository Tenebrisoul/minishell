#include "../minishell.h"

void	fill_env_with_value(char *env_str, t_env_item *node, int key_len)
{
	int	val_len;

	val_len = len(node->value);
	env_str[key_len] = '=';
	env_str[key_len + 1] = '"';
	if (val_len > 0)
		ft_strcpy(node->value, env_str + key_len + 2);
	env_str[key_len + val_len + 2] = '"';
	env_str[key_len + val_len + 3] = '\0';
}

void	fill_env_item(char **sorted_env, t_env_item *node, int *i)
{
	int	key_len;
	int	val_len;

	key_len = len(node->key);
	val_len = 0;
	if (node->value)
		val_len = len(node->value);
	sorted_env[*i] = alloc(key_len + val_len + 4);
	if (sorted_env[*i])
	{
		ft_strcpy(node->key, sorted_env[*i]);
		if (node->value)
			fill_env_with_value(sorted_env[*i], node, key_len);
		else
			sorted_env[*i][key_len] = '\0';
	}
	(*i)++;
}

void	bubble_sort_env(char **env_array, int count)
{
	char	*temp;
	int		i;
	int		j;

	i = 0;
	while (i < count - 1)
	{
		j = 0;
		while (j < count - 1 - i)
		{
			if (sh_strcmp(env_array[j], env_array[j + 1]) > 0)
			{
				temp = env_array[j];
				env_array[j] = env_array[j + 1];
				env_array[j + 1] = temp;
			}
			j++;
		}
		i++;
	}
}
