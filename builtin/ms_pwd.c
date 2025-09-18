/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ms_pwd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 16:55:17 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/18 16:11:24 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int bi_pwd(void)
{
	char buf[4096];
	char *p;

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