#include "shell.h"
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <stdio.h>

static char *handle_input(char *prompt)
{
	/* Check if we're in interactive mode */
	if (isatty(STDIN_FILENO) && isatty(STDERR_FILENO))
	{
		/* Interactive mode - use TTY handling with readline */
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
		/* Non-interactive mode - simple line reading with getline */
		char *line = NULL;
		size_t len = 0;
		ssize_t read_len = getline(&line, &len, stdin);
		if (read_len == -1)
		{
			if (line) free(line);
			return NULL;
		}
		/* Remove trailing newline */
		if (read_len > 0 && line[read_len - 1] == '\n')
			line[read_len - 1] = '\0';
		return line;
	}
}

/* Check if line was allocated by getline (non-interactive mode) */
static int is_getline_allocated(void)
{
	return !(isatty(STDIN_FILENO) && isatty(STDERR_FILENO));
}

int shell_run(t_shell *sh)
{
	char *line = NULL;
	char *prompt = "";

	if (isatty(2))
		prompt = "minishell$ ";
	sh_setup_signals();
	while (1)
	{
		line = handle_input(prompt);
		if (!line)
			break;
		if (sh_is_line_empty(line))
		{
			if (is_getline_allocated()) free(line);
			continue;
		}
		if (isatty(0))
			add_history(line);
		t_token *tokens = lexer(line);
		if (!tokens) {
			write(2, "minishell: syntax error\n", 24);
			sh->last_status = 2;
			if (is_getline_allocated()) free(line);
			continue;
		}

		t_ast_node *ast = parser(tokens);
		if (!ast)
		{
			write(2, "minishell: syntax error\n", 24);
			sh->last_status = 2;
			cleanup_tokens(tokens);
			if (is_getline_allocated()) free(line);
			continue;
		}
		int status = exec_ast(sh, ast);
		if (status >= 0)
			sh->last_status = status;
		cleanup_ast(ast);
		cleanup_tokens(tokens);
		if (is_getline_allocated()) free(line);
	}

	return sh ? sh->last_status : 0;
}

char *read_heredoc_input(t_shell *sh, const char *delimiter)
{
	char *content = (char*)gc_malloc(1024);
	int content_len = 0;
	int content_cap = 1024;
	char *line;
	int delimiter_len = sh_strlen(delimiter);
	
	if (!content)
		return NULL;
	content[0] = '\0';
	
	while (1)
	{
		/* Use appropriate input method based on mode */
		if (isatty(STDIN_FILENO))
		{
			line = readline("> ");
		}
		else
		{
			/* Non-interactive mode - use getline */
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
		
		if (!line) /* EOF reached (Ctrl+D) */
			break;
			
		/* Check if line matches delimiter exactly */
		if (sh_strlen(line) == delimiter_len && 
			sh_strncmp(line, delimiter, delimiter_len) == 0)
		{
			free(line);
			break;
		}
		
		/* Expand variables in heredoc content */
		char *expanded = expand_string(sh, line);
		if (!expanded)
			expanded = line;
		else
			free(line);
			
		int line_len = sh_strlen(expanded);
		
		/* Ensure we have enough space for line + newline + null terminator */
		while (content_len + line_len + 2 > content_cap)
		{
			content_cap *= 2;
			char *new_content = (char*)gc_malloc(content_cap);
			if (!new_content)
			{
				if (expanded != line) free(expanded);
				return content;
			}
			int i;
			for (i = 0; i < content_len; i++)
				new_content[i] = content[i];
			content = new_content;
		}
		
		/* Add line to content */
		int i;
		for (i = 0; i < line_len; i++)
			content[content_len++] = expanded[i];
		content[content_len++] = '\n';
		content[content_len] = '\0';
		
		if (expanded != line) free(expanded);
	}
	
	return content;
}

int handle_heredoc(t_shell *sh, const char *delimiter)
{
	char *content = read_heredoc_input(sh, delimiter);
	if (!content)
		return -1;
		
	/* Create a pipe to feed the content */
	int pipefd[2];
	if (pipe(pipefd) < 0)
	{
		perror("pipe");
		return -1;
	}
	
	/* Write content to pipe */
	int content_len = sh_strlen(content);
	if (write(pipefd[1], content, content_len) < 0)
	{
		perror("write");
		close(pipefd[0]);
		close(pipefd[1]);
		return -1;
	}
	close(pipefd[1]);
	
	/* Return read end of pipe for stdin redirection */
	return pipefd[0];
}
