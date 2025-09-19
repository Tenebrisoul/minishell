/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_command.c                                 :+:      :+:    :+:   */
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

static void	update_underscore_var(const t_command *cmd)
{
	t_env_item	*underscore_item;
	char		*last_arg;

	if (!cmd || cmd->argc <= 0 || !cmd->args)
		return ;
	last_arg = cmd->args[cmd->argc - 1];
	if (!last_arg)
		return ;
	underscore_item = new_env_item("_", last_arg);
	if (underscore_item)
		add_env_item(underscore_item);
}

int	exec_command(const t_command *cmd)
{
	int	rc;

	if (cmd->argc <= 0)
		return (0);
	if (sh_signal_interrupted())
	{
		reset_signals();
		return (130);
	}
	expand_args(cmd);
	expand_redirects(cmd);
	if (!cmd->args[0] || !cmd->args[0][0])
		return (0);
	update_underscore_var(cmd);
	if (is_builtin(cmd->args[0]))
	{
		if (cmd->redirects)
			return (exec_builtin_with_redir(cmd, cmd->args));
		else
		{
			rc = run_builtin(cmd->args);
			return (rc);
		}
	}
	return (exec_external_command(cmd, cmd->args));
}
