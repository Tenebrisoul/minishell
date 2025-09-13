#include "shell.h"
#include <signal.h>
#include <unistd.h>
#include <readline/readline.h>

static void sigint_handler(int signo)
{
	(void)signo;
	write(STDOUT_FILENO, "\n", 1);
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
}

void sh_setup_signals(void)
{
	struct sigaction sa_int;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = SA_RESTART;
	sa_int.sa_handler = sigint_handler;
	sigaction(SIGINT, &sa_int, NULL);

	signal(SIGQUIT, SIG_IGN);
}
