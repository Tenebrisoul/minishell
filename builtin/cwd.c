/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:23:47 by root              #+#    #+#             */
/*   Updated: 2025/09/19 02:23:48 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*get_cwd(void)
{
	char	cwd[8000];
	char	*response;
	int		counter;

	if (getcwd(cwd, sizeof(cwd)) == NULL)
		return (NULL);
	response = alloc(len(cwd) + 1);
	if (!response)
		return (NULL);
	counter = 0;
	while (cwd[counter])
	{
		response[counter] = cwd[counter];
		counter++;
	}
	response[counter] = '\0';
	return (response);
}

void	pwd(void)
{
	char	*cwd;

	cwd = NULL;
	cwd = get_cwd();
	if (!cwd)
		write(1, "pwd: couldn't fetch cwd.\n", 25);
	else
		write(1, cwd, len(cwd));
}

static char	*get_prefix(void)
{
	char	*cwd;
	char	*cwd_ptr;
	char	*prefix;
	int		counter;

	cwd = NULL;
	cwd_ptr = NULL;
	prefix = NULL;
	cwd = get_cwd();
	if (!cwd)
		return (NULL);
	cwd_ptr = cwd + len(cwd) - 1;
	while (*cwd_ptr)
	{
		if (*cwd_ptr == '/')
			break ;
		cwd_ptr--;
	}
	prefix = alloc(len(cwd_ptr) + 1);
	if (!prefix)
		return (NULL);
	counter = 0;
	while (*cwd_ptr)
	{
		prefix[counter] = *cwd_ptr;
		counter++;
		cwd_ptr++;
	}
	prefix[counter] = '\0';
	return (prefix);
}

void	print_prefix(void)
{
	char	*prefix;

	prefix = get_prefix();
	if (!prefix)
		return ;
	write(1, prefix, strlen(prefix));
	write(1, "/.", 2);
}
