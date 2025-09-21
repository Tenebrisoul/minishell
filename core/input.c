/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:11:24 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/21 23:30:08 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	setup_tty(int *saved_in, int *saved_out)
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

void	restore_tty(int saved_in, int saved_out)
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
	char	*line;

	setup_tty(&saved_in, &saved_out);
	line = readline(prompt);
	insert_to_gc(new_trash(line), GC_GC);
	restore_tty(saved_in, saved_out);
	return (line);
}
