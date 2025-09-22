/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:13:43 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/22 02:18:46 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include <readline/readline.h>
#include <signal.h>

volatile sig_atomic_t	g_shell_state = 0;

static void	sigint_handler(int signo)
{
	(void)signo;
	g_shell_state |= STATE_INTERRUPT;
	get_env()->exit_status = 130;
	if (g_shell_state & STATE_COMMAND)
		return ;
	if (g_shell_state & STATE_HEREDOC)
	{
		write(STDOUT_FILENO, "\n", 1);
		close(rl_instream->_fileno);
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
