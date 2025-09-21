/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_nodes.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:13:30 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/21 23:13:30 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_ast_node	*create_pipeline_node(t_ast_node *left, t_ast_node *right)
{
	t_ast_node	*pipeline;

	pipeline = create_ast_node(NODE_PIPELINE);
	if (!pipeline)
		return (NULL);
	pipeline->u_data.s_binary.left = left;
	pipeline->u_data.s_binary.right = right;
	return (pipeline);
}

t_ast_node	*create_sequence_node(t_ast_node *left, t_ast_node *right)
{
	t_ast_node	*sequence;

	sequence = create_ast_node(NODE_SEQUENCE);
	if (!sequence)
		return (NULL);
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
		return (NULL);
	node->u_data.command = cmd;
	return (node);
}
