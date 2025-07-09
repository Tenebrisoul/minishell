/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ivar <ivar@student.42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 23:49:12 by ivar              #+#    #+#             */
/*   Updated: 2025/07/08 23:49:13 by ivar             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include "stdlib.h"

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
	ast = parse_pipeline(parser);
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
