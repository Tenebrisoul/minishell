#include "../minishell.h"

static void	reset_signals(void)
{
	sh_signal_set_state(STATE_INTERRUPT, 0);
	sh_signal_set_state(STATE_HEREDOC, 0);
	sh_signal_set_state(STATE_COMMAND, 0);
}

int	shell_run(void)
{
	char	*line;
	char	*prompt;
	
	line = NULL;
	prompt = "";
	if (isatty(2))
		prompt = "minishell$ ";
	while (1)
	{
		dump_gc();
		get_gc(RESET_GC);
		reset_signals();
		line = handle_input(prompt);
		if (sh_signal_interrupted())
		{
			get_env()->exit_status = 130;
			reset_signals();
		}
		if (!line)
			break ;
		if (sh_is_line_empty(line))
			continue ;
		process_line(line);
	}
	if (isatty(STDIN_FILENO))
		printf("exit\n");
	return (get_env()->exit_status);
}
