#include "minishell.h"

__attribute__((constructor))
static void init_on_start(void)
{
    init_env();
}

__attribute__((destructor))
static void	cleanup_on_exit(void)
{
}

int	main(void)
{
	shell_run();
	return (get_env()->exit_status);
}
