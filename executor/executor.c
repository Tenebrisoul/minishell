#include "../minishell.h"

int	exec_external_command(const t_command *cmd, char **argv)
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return (1);
	}
	if (pid == 0)
		gc_exit(exec_child_process(cmd, argv));
	return (wait_for_child(pid));
}

int	exec_ast(const t_ast_node *ast)
{
	int	result;

	if (!ast)
		return (0);
	sh_signal_set_state(STATE_COMMAND, 1);
	result = 0;
	if (ast->type == NODE_COMMAND)
		result = exec_command(ast->u_data.command);
	else if (ast->type == NODE_SEQUENCE)
		result = exec_sequence(ast->u_data.s_binary.left,
				ast->u_data.s_binary.right);
	else if (ast->type == NODE_PIPELINE)
		result = exec_pipeline(ast->u_data.s_binary.left,
				ast->u_data.s_binary.right);
	sh_signal_set_state(STATE_COMMAND, 0);
	return (result);
}
