/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   repl.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:11:31 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/22 01:52:13 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	reset_signals(void)
{
	sh_signal_set_state(STATE_INTERRUPT, 0);
	sh_signal_set_state(STATE_HEREDOC, 0);
	sh_signal_set_state(STATE_COMMAND, 0);
}

static void	handle_line(char *line)
{
	reset_signals();
	if (!line)
		return ;
	if (sh_signal_interrupted())
	{
		get_env()->exit_status = 130;
		reset_signals();
	}
	if (sh_is_line_empty(line))
		return ;
	process_line(line);
}

int	shell_run(void)
{
	char	*line;
	char	*prompt;

	prompt = "";
	if (isatty(STDERR_FILENO))
		prompt = "minishell$ ";
	while (1)
	{
		dump_gc();
		get_gc(RESET_GC);
		line = handle_input(prompt);
		if (!line)
			break ;
		handle_line(line);
	}
	if (isatty(STDIN_FILENO))
		printf("exit\n");
	return (get_env()->exit_status);
}
