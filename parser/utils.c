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
	(void)redirects;
}

void	cleanup_command(t_command *cmd)
{
	(void)cmd;
}

void	cleanup_ast(t_ast_node *node)
{
	(void)node;
}
