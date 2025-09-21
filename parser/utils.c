#include "../minishell.h"

t_ast_node	*create_ast_node(t_node_type type)
{
	t_ast_node	*node;

	node = alloc(sizeof(t_ast_node));
	node->type = type;
	node->u_data.command = NULL;
	return (node);
}

void	advance_token(t_parser *parser)
{
	if (!parser || !parser->current)
		return ;
	parser->current = parser->current->next;
}
