#include "../minishell.h"

int	calc_total_expandables(void)
{
	int		total_len;
	char	**queue;
	int		i;

	total_len = 0;
	queue = get_expander(GET)->queue;
	i = 0;
	while (queue[i])
	{
		total_len += len(queue[i]) - 1;
		i++;
	}
	return (total_len + 1);
}

int	calc_total_expansions(void)
{
	t_env_item	*env_item;
	int			total_len;
	char		**queue;
	int			i;

	total_len = 0;
	queue = get_expander(GET)->queue;
	i = 0;
	while (queue[i])
	{
		env_item = is_env_item_exists(queue[i] + 1);
		if (env_item)
		{
			total_len += len(env_item->value);
		}
		i++;
	}
	return (total_len + 1);
}
