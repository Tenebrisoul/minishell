/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parentheses.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:23:40 by root              #+#    #+#             */
/*   Updated: 2025/09/19 02:46:50 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_ast_node	*parse_parentheses(t_parser *parser)
{
	t_ast_node	*node;
	t_ast_node	*child;

	if (!parser->current || parser->current->type != TOKEN_LPAREN)
		return (NULL);
	advance_token(parser);
	child = parse_expression(parser);
	if (!child)
		return (NULL);
	if (!parser->current || parser->current->type != TOKEN_RPAREN)
	{
		cleanup_ast(child);
		return (NULL);
	}
	advance_token(parser);
	node = create_ast_node(NODE_SUBSHELL);
	if (!node)
	{
		cleanup_ast(child);
		return (NULL);
	}
	node->u_data.s_subshell.child = child;
	return (node);
}

t_ast_node	*parse_primary(t_parser *parser)
{
	if (!parser->current)
		return (NULL);
	if (parser->current->type == TOKEN_LPAREN)
		return (parse_parentheses(parser));
	return (parse_ast_node(parser));
}

int	execute_subshell(const t_ast_node *node, t_shell *shell)
{
	pid_t	pid;
	int		status;

	(void)shell;
	if (!node || !node->u_data.s_subshell.child)
		return (1);
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (1);
	}
	if (pid == 0)
		exit(exec_ast(node->u_data.s_subshell.child));
	else
	{
		if (waitpid(pid, &status, 0) == -1)
		{
			perror("waitpid");
			return (1);
		}
		if (WIFEXITED(status))
			return (WEXITSTATUS(status));
		else if (WIFSIGNALED(status))
			return (128 + WTERMSIG(status));
		else
			return (1);
	}
}
