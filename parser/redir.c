/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ivar <ivar@student.42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 23:49:16 by ivar              #+#    #+#             */
/*   Updated: 2025/07/08 23:52:55 by ivar             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include "stdlib.h"

static t_redir_type	token_to_redir_type(t_token_type type)
{
	if (type == TOKEN_REDIR_IN)
		return (REDIR_IN);
	else if (type == TOKEN_REDIR_OUT)
		return (REDIR_OUT);
	else if (type == TOKEN_REDIR_APPEND)
		return (REDIR_APPEND);
	else if (type == TOKEN_HEREDOC)
		return (REDIR_HEREDOC);
	return (REDIR_IN);
}

static t_redirect	*create_redirect(t_parser *parser, t_token_type redir_token)
{
	t_redirect	*redirect;

	if (!parser->current || parser->current->type != TOKEN_WORD)
		return (NULL);
	redirect = malloc(sizeof(t_redirect));
	if (!redirect)
		return (NULL);
	redirect->type = token_to_redir_type(redir_token);
	redirect->filename = ft_strdup(parser->current->value);
	redirect->next = NULL;
	advance_token(parser);
	return (redirect);
}

bool	is_redirect_token(t_token_type type)
{
	if (type == TOKEN_REDIR_IN || type == TOKEN_REDIR_OUT
		|| type == TOKEN_REDIR_APPEND || type == TOKEN_HEREDOC)
		return (true);
	return (false);
}

t_redirect	*parse_redirections(t_parser *parser)
{
	t_redirect		*head;
	t_redirect		**next_ptr;
	t_redirect		*current;
	t_token_type	redir_token;

	head = NULL;
	next_ptr = &head;
	while (parser->current && is_redirect_token(parser->current->type))
	{
		redir_token = parser->current->type;
		advance_token(parser);
		current = create_redirect(parser, redir_token);
		if (!current)
		{
			cleanup_redirections(head);
			return (NULL);
		}
		*next_ptr = current;
		next_ptr = &current->next;
	}
	return (head);
}
