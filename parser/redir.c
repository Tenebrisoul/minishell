/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:13:37 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/22 02:12:16 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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

static int	setup_heredoc_redirect(t_redirect *redirect, t_parser *parser)
{
	char	*content;
	int		quoted;

	quoted = has_quotes(parser->current->value);
	redirect->quoted_delimiter = quoted;
	content = read_heredoc_input(parser->current->value);
	if (!content || (content && content[0] == '\0'
			&& get_env()->exit_status == 130))
		return (0);
	redirect->filename = content;
	return (1);
}

t_redirect	*create_redirect(t_parser *parser, t_token_type redir_token)
{
	t_redirect	*redirect;

	if (!parser->current || parser->current->type != TOKEN_WORD)
		return (NULL);
	redirect = alloc(sizeof(t_redirect));
	redirect->type = token_to_redir_type(redir_token);
	redirect->quoted_delimiter = 0;
	if (redir_token == TOKEN_HEREDOC)
	{
		if (!setup_heredoc_redirect(redirect, parser))
			return (NULL);
	}
	else
		redirect->filename = sh_strdup(parser->current->value);
	redirect->next = NULL;
	advance_token(parser);
	return (redirect);
}

t_redirect	*parse_redirections(t_parser *parser)
{
	t_redirect	*head;
	t_redirect	**next_ptr;
	t_redirect	*current;

	head = NULL;
	next_ptr = &head;
	while (parser->current && is_redirect_token(parser->current->type))
	{
		current = process_redirect_token(parser);
		if (!current)
			return (NULL);
		*next_ptr = current;
		next_ptr = &current->next;
	}
	return (head);
}
