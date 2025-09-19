#include "../minishell.h"

void	redirect_tty(int *tty, int *saved_in, int *saved_out)
{
	*saved_in = dup(STDIN_FILENO);
	*saved_out = dup(STDOUT_FILENO);
	*tty = open("/dev/tty", O_RDWR);
	if (*tty >= 0)
	{
		dup2(*tty, STDIN_FILENO);
		dup2(*tty, STDOUT_FILENO);
		close(*tty);
	}
	rl_instream = stdin;
	rl_outstream = stdout;
}

void	restore_stdio(int saved_in, int saved_out)
{
	dup2(saved_in, STDIN_FILENO);
	dup2(saved_out, STDOUT_FILENO);
	close(saved_in);
	close(saved_out);
	rl_instream = stdin;
	rl_outstream = stdout;
}

char	*handle_input(char *prompt)
{
	int		saved_in;
	int		saved_out;
	int		tty;
	char	*line;

	redirect_tty(&tty, &saved_in, &saved_out);
	line = readline(prompt);
	insert_to_gc(new_trash(line));
	if (tty >= 0)
		restore_stdio(saved_in, saved_out);
	else
	{
		close(saved_in);
		close(saved_out);
	}
	return (line);
}

int shell_run(void)
{
	char *line = NULL;
	char *prompt = "";

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
		if (sh_is_line_empty(line))
			continue ;
		if (isatty(0))
			add_history(line);
		t_token *tokens = lexer(line);
		if (!tokens) {
			write(2, "minishell: syntax error\n", 25);
			get_env()->exit_status = 2;
			continue;
		}
		t_ast_node *ast = parser(tokens);
		if (!ast)
		{
			if (get_env()->exit_status == 130)
			{
				cleanup_tokens(tokens);
				continue;
			}
			write(2, "minishell: syntax error\n", 25);
			get_env()->exit_status = 2;
			cleanup_tokens(tokens);
			continue;
		}
		if (sh_signal_interrupted())
		{
			printf("asd\n");
			continue;
		}
		int status = exec_ast(ast);
		if (status == 130)
			write(STDOUT_FILENO, "\n", 1);
		if (status >= 0)
			get_env()->exit_status = status;
	}

	return get_env()->exit_status;
}


