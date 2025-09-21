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

static int check_syntax_errors(t_token *tokens)
{
	(void)tokens;
	return (0);
}

int	process_line(char *line)
{
	t_token		*tokens;
	t_ast_node	*ast;
	int			status;

	if (isatty(0))
		add_history(line);
	if (is_getline_allocated())
		insert_to_gc(new_trash(line), GC_GC);
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
