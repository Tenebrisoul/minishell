#include "../minishell.h"

t_ast_node			*parse_pipeline(t_parser *parser);

static t_parser	*init_parser(t_token *tokens)
{
	t_parser	*parser;

	if (!tokens)
		return (NULL);
	parser = alloc(sizeof(t_parser));
	parser->tokens = tokens;
	parser->current = tokens;
	return (parser);
}

t_ast_node	*parse_sequence(t_parser *parser)
{
	t_ast_node	*left;
	t_ast_node	*right;

	left = parse_pipeline(parser);
	if (!left)
		return (NULL);
	while (parser->current && parser->current->type == TOKEN_SEMICOLON)
	{
		advance_token(parser);
		right = parse_pipeline(parser);
		if (!right)
			return (NULL);
		left = create_sequence_node(left, right);
		if (!left)
			return (NULL);
	}
	return (left);
}

t_ast_node	*parse_pipeline(t_parser *parser)
{
	t_ast_node	*left;
	t_ast_node	*right;

	left = parse_ast_node(parser);
	if (!left)
		return (NULL);
	while (parser->current && parser->current->type == TOKEN_PIPE)
	{
		advance_token(parser);
		right = parse_ast_node(parser);
		if (!right)
			return (NULL);
		left = create_pipeline_node(left, right);
		if (!left)
			return (NULL);
	}
	return (left);
}

t_ast_node	*parser(t_token *tokens)
{
	t_ast_node	*ast;
	t_parser	*parser;

	if (!tokens)
		return (NULL);
	parser = init_parser(tokens);
	if (!parser)
		return (NULL);
	ast = parse_sequence(parser);
	return (ast);
}
