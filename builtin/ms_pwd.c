/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ms_pwd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:11:04 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/22 01:51:59 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	bi_pwd(void)
{
	char	buf[4096];
	char	*p;

	p = getcwd(buf, sizeof(buf));
	if (!p)
	{
		write(2, "minishell: pwd: error retrieving current directory\n", 51);
		return (1);
	}
	write(1, p, sh_strlen(p));
	write(1, "\n", 1);
	return (0);
}
