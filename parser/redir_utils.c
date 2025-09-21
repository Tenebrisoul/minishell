/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:13:34 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/22 02:12:23 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

bool	is_redirect_token(t_token_type type)
{
	if (type == TOKEN_REDIR_IN || type == TOKEN_REDIR_OUT
		|| type == TOKEN_REDIR_APPEND || type == TOKEN_HEREDOC)
		return (true);
	return (false);
}

static t_redirect	*handle_consecutive_heredocs(t_parser *parser)
{
	t_redirect		*last_heredoc;
	t_token_type	redir_token;

	last_heredoc = NULL;
	while (parser->current && parser->current->type == TOKEN_HEREDOC)
	{
		redir_token = parser->current->type;
		advance_token(parser);
		last_heredoc = create_redirect(parser, redir_token);
		if (!last_heredoc)
			return (NULL);
	}
	return (last_heredoc);
}

t_redirect	*process_redirect_token(t_parser *parser)
{
	t_redirect		*current;
	t_token_type	redir_token;

	if (parser->current->type == TOKEN_HEREDOC)
		current = handle_consecutive_heredocs(parser);
	else
	{
		redir_token = parser->current->type;
		advance_token(parser);
		current = create_redirect(parser, redir_token);
		get_env()->exit_status = 2;
	}
	return (current);
}
