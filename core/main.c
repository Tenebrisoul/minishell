#include "shell.h"

__attribute__((destructor))
static void	cleanup_on_exit(void)
{
	gc_cleanup();
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	sh;
	int		code;

	gc_init();
	(void)argc; 
	(void)argv;
	if (sh_init(&sh, envp) != 0)
		return 1;
	code = shell_run(&sh);
	return code;
}
