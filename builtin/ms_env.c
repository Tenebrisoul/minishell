#include "../minishell.h"

int	bi_env(char **argv)
{
	(void)argv;
	print_env();
	return (0);
}
