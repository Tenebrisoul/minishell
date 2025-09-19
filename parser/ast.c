/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:25:14 by root              #+#    #+#             */
/*   Updated: 2025/09/19 12:19:53 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static t_command	*init_command(t_parser *parser)
{
	t_token		*current;
	t_command	*cmd;
	size_t		size;

	cmd = alloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	size = 0;
	current = parser->current;
	while (current && current->type != TOKEN_PIPE
		&& current->type != TOKEN_SEMICOLON && current->type != TOKEN_AND
		&& current->type != TOKEN_OR && current->type != TOKEN_RPAREN)
	{
		size++;
		current = current->next;
	}
	cmd->args = alloc(sizeof(char *) * (size + 1));
	if (!cmd->args)
	{
		free(cmd);
		return (NULL);
	}
	cmd->argc = 0;
	cmd->args[0] = NULL;
	cmd->redirects = NULL;
	return (cmd);
}

static void	add_redirect_to_command(t_command *cmd, t_redirect *redir)
{
	t_redirect	*current;

	if (!cmd || !redir)
		return ;
	if (!cmd->redirects)
	{
		cmd->redirects = redir;
		return ;
	}
	current = cmd->redirects;
	while (current->next)
		current = current->next;
	current->next = redir;
}

static int	process_token(t_parser *parser, t_command *cmd)
{
	t_redirect	*redirects;

	if (is_redirect_token(parser->current->type))
	{
		redirects = parse_redirections(parser);
		if (!redirects)
			return (0);
		add_redirect_to_command(cmd, redirects);
	}
	else if (parser->current->type == TOKEN_WORD
		|| parser->current->type == TOKEN_WILDCARD)
	{
		if (!cmd || !parser->current->value)
			return (0);
		cmd->args[cmd->argc] = ft_strdup(parser->current->value);
		if (!cmd->args[cmd->argc])
			return (0);
		cmd->argc++;
		cmd->args[cmd->argc] = NULL;
		advance_token(parser);
	}
	else
		return (-1);
	return (1);
}

static t_command	*parse_command(t_parser *parser)
{
	t_command	*cmd;
	int			result;

	if (!parser || !parser->current)
		return (NULL);
	cmd = init_command(parser);
	if (!cmd)
		return (NULL);
	while (parser->current && parser->current->type != TOKEN_PIPE
		&& parser->current->type != TOKEN_SEMICOLON
		&& parser->current->type != TOKEN_AND
		&& parser->current->type != TOKEN_OR
		&& parser->current->type != TOKEN_RPAREN)
	{
		result = process_token(parser, cmd);
		if (result == 0)
		{
			cleanup_command(cmd);
			return (NULL);
		}
		else if (result == -1)
			break ;
	}
	if (cmd->argc == 0 && !cmd->redirects)
	{
		cleanup_command(cmd);
		return (NULL);
	}
	return (cmd);
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
	{
		cleanup_command(cmd);
		return (NULL);
	}
	node->u_data.command = cmd;
	return (node);
}
