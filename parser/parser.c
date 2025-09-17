#include "../minishell.h"
#include <stdlib.h>
#include <stddef.h>

static t_ast_node	*parse_pipeline(t_parser *parser);

static t_parser	*init_parser(t_token *tokens)
{
	t_parser	*parser;

	if (!tokens)
		return (NULL);
	parser = malloc(sizeof(t_parser));
	if (!parser)
		return (NULL);
	parser->tokens = tokens;
	parser->current = tokens;
	return (parser);
}

static t_ast_node	*create_pipeline_node(t_ast_node *left, t_ast_node *right)
{
	t_ast_node	*pipeline;

	pipeline = create_ast_node(NODE_PIPELINE);
	if (!pipeline)
	{
		cleanup_ast(left);
		cleanup_ast(right);
		return (NULL);
	}
	pipeline->u_data.s_pipeline.left = left;
	pipeline->u_data.s_pipeline.right = right;
	return (pipeline);
}

static t_ast_node	*create_sequence_node(t_ast_node *left, t_ast_node *right)
{
	t_ast_node	*sequence;

	sequence = create_ast_node(NODE_SEQUENCE);
	if (!sequence)
	{
		cleanup_ast(left);
		cleanup_ast(right);
		return (NULL);
	}
	sequence->u_data.s_pipeline.left = left;
	sequence->u_data.s_pipeline.right = right;
	return (sequence);
}

static t_ast_node	*parse_sequence(t_parser *parser)
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
		{
			cleanup_ast(left);
			return (NULL);
		}
		left = create_sequence_node(left, right);
		if (!left)
			return (NULL);
	}
	return (left);
}

static t_ast_node	*parse_pipeline(t_parser *parser)
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
		{
			cleanup_ast(left);
			return (NULL);
		}
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
	if (!ast)
	{
		if (parser)
			free(parser);
		return (NULL);
	}
	if (parser)
		free(parser);
	return (ast);
}
