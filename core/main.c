#include "minishell.h"
#include "../environment/env.h"

__attribute__((constructor))
static void init_on_start(void)
{
    init_env();
	// init_signals();
}

__attribute__((destructor))
static void	cleanup_on_exit(void)
{
	// gc_cleanup();
}

int shell_run();
int	main(void)
{
	shell_run();
	return (get_env()->exit_status);
}
