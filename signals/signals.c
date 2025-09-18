#include "../minishell.h"
#include <stdlib.h>
#include <stddef.h>

static volatile sig_atomic_t	g_shell_state = 0;

#define STATE_NORMAL	0
#define STATE_INTERRUPT	1
#define STATE_HEREDOC	2
#define STATE_COMMAND	4

static void	sigint_handler(int signo)
{
	(void)signo;
	g_shell_state |= STATE_INTERRUPT;

	if (g_shell_state & STATE_HEREDOC)
	{
		write(STDOUT_FILENO, "\n", 1);
		rl_done = 1;
		return ;
	}

	if (g_shell_state & STATE_COMMAND)
	{
		write(STDOUT_FILENO, "\n", 1);
		return ;
	}

	write(STDOUT_FILENO, "\n", 1);
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
}

void	init_signals(void)
{
	struct sigaction	sa_int;

	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = 0;
	sa_int.sa_handler = sigint_handler;
	sigaction(SIGINT, &sa_int, NULL);
	signal(SIGQUIT, SIG_IGN);
}

void	sh_signal_set_state(int state_type, int value)
{
	if (value)
		g_shell_state |= state_type;
	else
		g_shell_state &= ~state_type;
}

int	sh_signal_interrupted(void)
{
	return ((g_shell_state & STATE_INTERRUPT) != 0);
}

int	sh_signal_should_exit(void)
{
	return ((g_shell_state & STATE_INTERRUPT) && !(g_shell_state & (STATE_HEREDOC | STATE_COMMAND)));
}

void	sh_signal_reset(void)
{
	g_shell_state = STATE_NORMAL;
}
