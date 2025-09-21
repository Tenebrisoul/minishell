#include "../minishell.h"

int	is_getline_allocated(void)
{
	return (!(isatty(STDIN_FILENO) && isatty(STDERR_FILENO)));
}
