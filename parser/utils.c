/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ivar <ivar@student.42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 23:49:18 by ivar              #+#    #+#             */
/*   Updated: 2025/07/08 23:49:19 by ivar             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include <stdlib.h>

t_ast_node	*create_ast_node(t_node_type type)
{
	t_ast_node	*node;

	node = malloc(sizeof(t_ast_node));
	if (!node)
		return (NULL);
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

void	cleanup_redirections(t_redirect *redirects)
{
	t_redirect	*next;

	while (redirects)
	{
		next = redirects->next;
		free(redirects->filename);
		free(redirects);
		redirects = next;
	}
}

void	cleanup_command(t_command *cmd)
{
	int	i;

	if (!cmd)
		return ;
	i = 0;
	if (cmd->args)
	{
		while (i < cmd->argc)
		{
			free(cmd->args[i]);
			i++;
		}
		free(cmd->args);
	}
	cleanup_redirections(cmd->redirects);
	free(cmd);
}

void	cleanup_ast(t_ast_node *node)
{
	if (!node)
		return ;
	if (node->type == NODE_COMMAND)
		cleanup_command(node->u_data.command);
	else if (node->type == NODE_PIPELINE)
	{
		cleanup_ast(node->u_data.s_pipeline.left);
		cleanup_ast(node->u_data.s_pipeline.right);
	}
	free(node);
}

