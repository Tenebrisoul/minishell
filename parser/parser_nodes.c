#include "../minishell.h"

t_ast_node	*create_pipeline_node(t_ast_node *left, t_ast_node *right)
{
	t_ast_node	*pipeline;

	pipeline = create_ast_node(NODE_PIPELINE);
	if (!pipeline)
	{
		cleanup_ast(left);
		cleanup_ast(right);
		return (NULL);
	}
	pipeline->u_data.s_binary.left = left;
	pipeline->u_data.s_binary.right = right;
	return (pipeline);
}

t_ast_node	*create_sequence_node(t_ast_node *left, t_ast_node *right)
{
	t_ast_node	*sequence;

	sequence = create_ast_node(NODE_SEQUENCE);
	if (!sequence)
	{
		cleanup_ast(left);
		cleanup_ast(right);
		return (NULL);
	}
	sequence->u_data.s_binary.left = left;
	sequence->u_data.s_binary.right = right;
	return (sequence);
}

t_ast_node	*parse_ast_node(t_parser *parser)
{
	t_command	*cmd;
	t_ast_node	*node;

	cmd = parse_command(parser);
	if (!cmd)
		return (NULL);
	node = create_ast_node(NODE_COMMAND);
	if (!node)
	{
		cleanup_command(cmd);
		return (NULL);
	}
	node->u_data.command = cmd;
	return (node);
}
