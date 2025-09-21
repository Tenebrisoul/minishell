#include "../minishell.h"

__attribute__((constructor)) static void	init_on_start(void)
{
	init_env();
	init_signals();
}

__attribute__((destructor)) static void	cleanup_on_exit(void)
{
	t_env	*env;
	int		exit_status;

	env = get_env();
	if (env)
		exit_status = get_env()->exit_status;
	else
		exit_status = 0;
	dump_gc();
	get_gc(RESET_GC);
	dump_env_gc();
	get_env_gc(RESET_GC);
	rl_clear_history();
	exit(exit_status);
}

int	main(void)
{
	shell_run();
	return (get_env()->exit_status);
}
