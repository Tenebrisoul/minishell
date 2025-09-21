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

static int	execute_builtin_or_empty(const t_command *cmd, int is_builtin)
{
	int	saved_stdin;
	int	saved_stdout;
	int	result;

	if (!is_builtin)
	{
		if (cmd->redirects)
		{
			saved_stdin = dup(0);
			saved_stdout = dup(1);
			result = apply_redirs(cmd->redirects);
			dup2(saved_stdin, 0);
			dup2(saved_stdout, 1);
			close(saved_stdin);
			close(saved_stdout);
			return (result);
		}
		return (0);
	}
	if (cmd->redirects)
		return (exec_builtin_with_redir(cmd, cmd->args));
	return (run_builtin(cmd->args));
}

static int	prepare_command_execution(const t_command *cmd)
{
	if (sh_signal_interrupted())
	{
		reset_signals();
		return (130);
	}
	expand_args(cmd);
	expand_redirects(cmd);
	if (!cmd->args[0])
		return (0);
	return (-1);
}

int	exec_command(const t_command *cmd)
{
	int	prep_result;

	if (cmd->argc <= 0)
		return (execute_builtin_or_empty(cmd, 0));
	prep_result = prepare_command_execution(cmd);
	if (prep_result != -1)
		return (prep_result);
	update_underscore_var(cmd);
	if (is_builtin(cmd->args[0]))
		return (execute_builtin_or_empty(cmd, 1));
	return (exec_external_command(cmd, cmd->args));
}
