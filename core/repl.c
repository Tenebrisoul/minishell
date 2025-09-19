/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   repl.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:24:04 by root              #+#    #+#             */
/*   Updated: 2025/09/19 13:25:41 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"


int	shell_run(void)
{
	char		*line;
	char		*prompt;
	t_token		*tokens;
	t_ast_node	*ast;
	int			status;

	line = NULL;
	prompt = "";
	if (isatty(2))
		prompt = "minishell$ ";
	while (1)
	{
		sh_signal_reset();
		line = handle_input(prompt);
		if (sh_signal_interrupted())
		{
			get_env()->exit_status = 130;
			sh_signal_reset();
		}
		if (!line)
			break ;
		if (sh_is_line_empty(remove_outer_quotes(line)))
			continue ;
		if (isatty(0))
			add_history(line);
		if (is_getline_allocated())
			insert_to_gc(new_trash(line));
		tokens = lexer(line);
		if (!tokens)
		{
			write(2, "minishell: syntax error\n", 25);
			get_env()->exit_status = 2;
			continue ;
		}
		ast = parser(tokens);
		if (!ast)
		{
			if (get_env()->exit_status == 130)
			{
				cleanup_tokens(tokens);
				continue ;
			}
			write(2, "minishell: syntax error\n", 25);
			get_env()->exit_status = 2;
			cleanup_tokens(tokens);
			continue ;
		}
		if (sh_signal_interrupted())
		{
			continue ;
		}
		status = exec_ast(ast);
		if (status == 130)
			write(STDOUT_FILENO, "\n", 1);
		if (status >= 0)
			get_env()->exit_status = status;
	}
	return (get_env()->exit_status);
}
