/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:13:39 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/22 02:12:31 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
