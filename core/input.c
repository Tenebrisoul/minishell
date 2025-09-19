/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/01 00:00:00 by student           #+#    #+#             */
/*   Updated: 2024/01/01 00:00:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	setup_tty(int *saved_in, int *saved_out)
{
	int	tty;

	*saved_in = dup(STDIN_FILENO);
	*saved_out = dup(STDOUT_FILENO);
	tty = open("/dev/tty", O_RDWR);
	if (tty >= 0)
	{
		dup2(tty, STDIN_FILENO);
		dup2(tty, STDOUT_FILENO);
		close(tty);
	}
	rl_instream = stdin;
	rl_outstream = stdout;
}

static void	restore_tty(int saved_in, int saved_out)
{
	dup2(saved_in, STDIN_FILENO);
	dup2(saved_out, STDOUT_FILENO);
	close(saved_in);
	close(saved_out);
	rl_instream = stdin;
	rl_outstream = stdout;
}

static char	*handle_tty_input(char *prompt)
{
	int		saved_in;
	int		saved_out;
	char	*line;

	setup_tty(&saved_in, &saved_out);
	line = readline(prompt);
	restore_tty(saved_in, saved_out);
	return (line);
}

static char	*handle_pipe_input(void)
{
	char	*line;
	size_t	len;
	ssize_t	read_len;

	line = NULL;
	len = 0;
	read_len = getline(&line, &len, stdin);
	if (read_len == -1)
	{
		return (NULL);
	}
	if (read_len > 0 && line[read_len - 1] == '\n')
		line[read_len - 1] = '\0';
	return (line);
}

char	*handle_input(char *prompt)
{
	if (isatty(STDIN_FILENO) && isatty(STDERR_FILENO))
		return (handle_tty_input(prompt));
	else
	{
		return (handle_pipe_input());
	}
}

int	is_getline_allocated(void)
{
	return (!(isatty(STDIN_FILENO) && isatty(STDERR_FILENO)));
}
