#include "../minishell.h"

void	setup_tty(int *saved_in, int *saved_out)
{
	int	tty;

	*saved_in = dup(STDIN_FILENO);
	*saved_out = dup(STDOUT_FILENO);
	tty = open("/dev/tty", O_RDWR);
	if (tty >= 0)
	{
		dup2(tty, STDIN_FILENO);
		dup2(tty, STDOUT_FILENO);
		close(tty);
	}
	rl_instream = stdin;
	rl_outstream = stdout;
}

void	restore_tty(int saved_in, int saved_out)
{
	dup2(saved_in, STDIN_FILENO);
	dup2(saved_out, STDOUT_FILENO);
	close(saved_in);
	close(saved_out);
	rl_instream = stdin;
	rl_outstream = stdout;
}

static char	*handle_pipe_input(void)
{
	char	*line;

	rl_outstream = fopen("/dev/null", "w");
	line = readline(NULL);
	if (rl_outstream != stdout)
	{
		fclose(rl_outstream);
		rl_outstream = stdout;
	}
	if (!line)
		return (NULL);
	insert_to_gc(new_trash(line), GC_GC);
	return (line);
}

static char	*handle_tty_input(char *prompt)
{
	int		saved_in;
	int		saved_out;
	char	*line;

	setup_tty(&saved_in, &saved_out);
	line = readline(prompt);
	insert_to_gc(new_trash(line), GC_GC);
	restore_tty(saved_in, saved_out);
	return (line);
}

char	*handle_input(char *prompt)
{
	if (isatty(STDIN_FILENO) && isatty(STDERR_FILENO))
		return (handle_tty_input(prompt));
	else
		return (handle_pipe_input());
}
