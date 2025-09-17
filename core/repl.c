#include "minishell.h"

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
		line = handle_input(prompt);
		if (!line)
			break;
		if (sh_is_line_empty(line))
		{
			if (is_getline_allocated())
				free(line);
			continue;
		}
		if (isatty(0))
			add_history(line);
		t_token *tokens = lexer(line);
		if (!tokens) {
			write(2, "minishell: syntax error\n", 24);
			get_env()->exit_status = 2;
			if (is_getline_allocated())
				free(line);
			continue;
		}

		t_ast_node *ast = parser(tokens);
		if (!ast)
		{
			write(2, "minishell: syntax error\n", 24);
			get_env()->exit_status = 2;
			cleanup_tokens(tokens);
			if (is_getline_allocated())
				free(line);
			continue;
		}
		int status = exec_ast(ast);
		if (status >= 0)
			get_env()->exit_status = status;
		
		sh_signal_reset();
		
		cleanup_ast(ast);
		cleanup_tokens(tokens);
		if (is_getline_allocated()) free(line);
	}

	return get_env()->exit_status;
}

char *read_heredoc_input(const char *delimiter)
{
	char *content = (char*)alloc(1024);
	int content_len = 0;
	int content_cap = 1024;
	char *line;
	int delimiter_len = sh_strlen(delimiter);
	
	if (!content)
		return NULL;
	content[0] = '\0';
	
	sh_signal_set_state(STATE_HEREDOC, 1);
	
	while (1)
	{
		if (sh_signal_interrupted()) {
			sh_signal_reset();
			sh_signal_set_state(STATE_HEREDOC, 0);
			return NULL;
		}
		
		if (isatty(STDIN_FILENO))
		{
			line = readline("> ");
		}
		else
		{
			size_t len = 0;
			ssize_t read_len = getline(&line, &len, stdin);
			if (read_len == -1)
			{
				if (line) free(line);
				line = NULL;
			}
			else if (read_len > 0 && line[read_len - 1] == '\n')
				line[read_len - 1] = '\0';
		}
		
		if (!line)
		{
			sh_signal_set_state(STATE_HEREDOC, 0);
			break;
		}
			
		if (sh_strlen(line) == delimiter_len && 
			sh_strncmp(line, delimiter, delimiter_len) == 0)
		{
			free(line);
			sh_signal_set_state(STATE_HEREDOC, 0);
			break;
		}
		
		char *expanded = expand_string(line);
		if (!expanded)
			expanded = line;
		else
			free(line);
			
		int line_len = sh_strlen(expanded);
		
		while (content_len + line_len + 2 > content_cap)
		{
			content_cap *= 2;
			char *new_content = (char*)alloc(content_cap);
			if (!new_content)
			{
				if (expanded != line) free(expanded);
				return content;
			}
			int i;

			i = 0;
			while (i < content_len)
			{
				new_content[i] = content[i];
				i++;
			}
			content = new_content;
		}
		
		int i;

		i = 0;
		while (i < line_len)
		{
			content[content_len++] = expanded[i];
			i++;
		}
		content[content_len++] = '\n';
		content[content_len] = '\0';
		
		if (expanded != line) free(expanded);
	}
	
	return content;
}

int handle_heredoc(const char *delimiter)
{
	char *content = read_heredoc_input(delimiter);
	if (!content)
		return -1;
		
	int pipefd[2];
	if (pipe(pipefd) < 0)
	{
		perror("pipe");
		return -1;
	}
	
	int content_len = sh_strlen(content);
	if (write(pipefd[1], content, content_len) < 0)
	{
		perror("write");
		close(pipefd[0]);
		close(pipefd[1]);
		return -1;
	}
	close(pipefd[1]);
	
	return pipefd[0];
}
