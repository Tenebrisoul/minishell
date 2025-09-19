/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:25:21 by root              #+#    #+#             */
/*   Updated: 2025/09/19 12:20:38 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_ast_node	*create_ast_node(t_node_type type)
{
	t_ast_node	*node;

	node = alloc(sizeof(t_ast_node));
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
	else if (node->type == NODE_PIPELINE || node->type == NODE_SEQUENCE
		|| node->type == NODE_AND || node->type == NODE_OR)
	{
		cleanup_ast(node->u_data.s_binary.left);
		cleanup_ast(node->u_data.s_binary.right);
	}
	else if (node->type == NODE_SUBSHELL)
	{
		cleanup_ast(node->u_data.s_subshell.child);
	}
	free(node);
}
