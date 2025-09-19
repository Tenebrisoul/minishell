/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:23:47 by root              #+#    #+#             */
/*   Updated: 2025/09/19 20:54:12 by root             ###   ########.fr       */
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
