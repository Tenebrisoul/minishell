/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   repl_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:11:29 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/22 01:52:10 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	handle_lexer_error(void)
{
	write(2, "minishell: syntax error\n", 25);
	get_env()->exit_status = 2;
	return (1);
}

static int	handle_parser_error(void)
{
	if (get_env()->exit_status == 130)
		return (1);
	write(2, "minishell: syntax error\n", 25);
	get_env()->exit_status = 2;
	return (1);
}

static int	is_redir_token(t_token_type type)
{
	return (type == TOKEN_REDIR_IN || type == TOKEN_REDIR_OUT
		|| type == TOKEN_REDIR_APPEND || type == TOKEN_HEREDOC);
}

static int	check_syntax_errors(t_token *tokens)
{
	t_token	*current;

	(void)tokens;
	current = tokens;
	while (current)
	{
		if (is_redir_token(current->type))
		{
			if (!current->next || current->next->type != TOKEN_WORD)
				return (1);
		}
		current = current->next;
	}
	return (0);
}

int	process_line(char *line)
{
	t_token		*tokens;
	t_ast_node	*ast;
	int			status;

	if (isatty(0))
		add_history(line);
	tokens = lexer(line);
	if (!tokens)
		return (handle_lexer_error());
	if (check_syntax_errors(tokens))
		return (handle_lexer_error());
	ast = parser(tokens);
	if (!ast)
		return (handle_parser_error());
	if (sh_signal_interrupted())
		return (1);
	status = exec_ast(ast);
	if (status == 130)
		write(STDOUT_FILENO, "\n", 1);
	if (status >= 0)
		get_env()->exit_status = status;
	return (1);
}
