/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/01 00:00:00 by student           #+#    #+#             */
/*   Updated: 2024/01/01 00:00:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include <stdlib.h>
#include <stddef.h>

t_parser	*init_parser(t_token *tokens)
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
	pipeline->u_data.s_pipeline.left = left;
	pipeline->u_data.s_pipeline.right = right;
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
	sequence->u_data.s_pipeline.left = left;
	sequence->u_data.s_pipeline.right = right;
	return (sequence);
}
