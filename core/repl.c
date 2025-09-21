/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   repl.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:24:04 by root              #+#    #+#             */
/*   Updated: 2025/09/21 00:08:04 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
		if (sh_is_line_empty(remove_outer_quotes(line)))
			get_env()->exit_status = 127;
		if (sh_is_line_empty(remove_outer_quotes(line)))
			continue ;
		process_line(line);
	}
	return (get_env()->exit_status);
}
