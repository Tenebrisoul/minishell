#include "../minishell.h"
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <stdio.h>

static char *handle_input(char *prompt)
{
	if (isatty(STDIN_FILENO) && isatty(STDERR_FILENO))
	{
		int saved_in = dup(STDIN_FILENO);
		int saved_out = dup(STDOUT_FILENO);
		int tty = open("/dev/tty", O_RDWR);
		
		if (tty >= 0)
		{
			dup2(tty, STDIN_FILENO);
			dup2(tty, STDOUT_FILENO);
			close(tty);
		}
		rl_instream = stdin;
		rl_outstream = stdout;
		char *line = readline(prompt);

		if (!line)
		{
			if (sh_signal_interrupted())
			{
				get_env()->exit_status = 130;
				if (tty >= 0)
				{
					dup2(saved_in, STDIN_FILENO);
					dup2(saved_out, STDOUT_FILENO);
				}
				close(saved_in);
				close(saved_out);
				rl_instream = stdin;
				rl_outstream = stdout;
				return (ft_strdup(""));
			}
			if (tty >= 0)
			{
				dup2(saved_in, STDIN_FILENO);
				dup2(saved_out, STDOUT_FILENO);
			}
			close(saved_in);
			close(saved_out);
			rl_instream = stdin;
			rl_outstream = stdout;
			return (NULL);
		}

		if (tty >= 0)
		{
			dup2(saved_in, STDIN_FILENO);
			dup2(saved_out, STDOUT_FILENO);
		}
		close(saved_in);
		close(saved_out);
		rl_instream = stdin;
		rl_outstream = stdout;
		return (line);
	}
	else
	{
		char *line = NULL;
		size_t len = 0;
		ssize_t read_len = getline(&line, &len, stdin);
		if (read_len == -1)
		{
			if (line) free(line);
			return NULL;
		}
		if (read_len > 0 && line[read_len - 1] == '\n')
			line[read_len - 1] = '\0';
		return line;
	}
}

static int is_getline_allocated(void)
{
	return !(isatty(STDIN_FILENO) && isatty(STDERR_FILENO));
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
		if (!line)
			break;

		if (!line || sh_is_line_empty(line))
		{
			if (is_getline_allocated())
				free(line);
			continue;
		}
		if (isatty(0))
			add_history(line);
		t_token *tokens = lexer(line);
		if (!tokens) {
			write(2, "minishell: syntax error\n", 25);
			get_env()->exit_status = 2;
			if (is_getline_allocated())
				free(line);
			continue;
		}

		t_ast_node *ast = parser(tokens);
		if (!ast)
		{
			write(2, "minishell: syntax error\n", 25);
			get_env()->exit_status = 2;
			cleanup_tokens(tokens);
			if (is_getline_allocated())
				free(line);
			continue;
		}
		int status = exec_ast(ast);
		if (status >= 0)
			get_env()->exit_status = status;

		if (sh_signal_interrupted())
		{
			get_env()->exit_status = 130;
			sh_signal_reset();
		}

		cleanup_ast(ast);
		cleanup_tokens(tokens);
		if (is_getline_allocated()) free(line);
	}

	return get_env()->exit_status;
}


