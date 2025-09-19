/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_sequence.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 21:10:00 by root              #+#    #+#             */
/*   Updated: 2025/09/19 21:10:00 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	reset_signals(void)
{
	sh_signal_set_state(STATE_INTERRUPT, 0);
	sh_signal_set_state(STATE_HEREDOC, 0);
	sh_signal_set_state(STATE_COMMAND, 0);
}

int	exec_sequence(const t_ast_node *left, const t_ast_node *right)
{
	int	left_status;

	if (sh_signal_interrupted())
	{
		reset_signals();
		return (130);
	}
	left_status = exec_ast(left);
	get_env()->exit_status = left_status;
	if (sh_signal_interrupted())
	{
		reset_signals();
		return (130);
	}
	return (exec_ast(right));
}
