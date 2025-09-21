#include "../minishell.h"


static char	*handle_non_tty_heredoc(void)
{
	char	*line;

	line = readline(NULL);
	if (!line)
		return (NULL);
	insert_to_gc(new_trash(line), GC_GC);
	return (line);
}

char	*get_heredoc_line(void)
{
	char	*line;

	if (isatty(STDIN_FILENO))
	{
		if (sh_signal_interrupted())
			return (NULL);
		line = safe_heredoc("> ");
		if (!line || sh_signal_interrupted())
			return (NULL);
		return (line);
	}
	return (handle_non_tty_heredoc());
}

char	*safe_heredoc(char *prompt)
{
	int		saved_in;
	int		saved_out;
	char	*line;

	setup_tty(&saved_in, &saved_out);
	line = readline(prompt);
	insert_to_gc(new_trash(line), GC_GC);
	restore_tty(saved_in, saved_out);
	return (line);
}

void	print_heredoc_warning(char *delimiter)
{
	write(2, "minishell: warning: here-document ", 35);
	write(2, "delimited by end-of-file (wanted `", 35);
	write(2, delimiter, sh_strlen(delimiter));
	write(2, "')\n", 3);
}
