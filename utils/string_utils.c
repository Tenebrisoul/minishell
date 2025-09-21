/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:13:46 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/21 23:13:47 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	sh_is_line_empty(const char *s)
{
	size_t	i;

	i = 0;
	if (!s)
		return (1);
	while (s[i])
	{
		if (s[i] != ' ' && s[i] != '\t' && s[i] != '\n' && s[i] != '\r')
			return (0);
		i++;
	}
	return (1);
}

int	sh_strlen(const char *s)
{
	int	i;

	i = 0;
	if (!s)
		return (0);
	while (s[i])
		i++;
	return (i);
}

char	*sh_strdup(const char *s)
{
	int		len;
	char	*p;
	int		i;

	len = sh_strlen(s);
	p = (char *)alloc((len + 1) * sizeof(char));
	i = 0;
	while (i < len)
	{
		p[i] = s[i];
		i++;
	}
	p[len] = '\0';
	return (p);
}

int	sh_strcmp(const char *a, const char *b)
{
	int	i;

	i = 0;
	if (!a || !b)
	{
		if (a == b)
			return (0);
		if (a)
			return (1);
		return (-1);
	}
	while (a[i] && b[i] && a[i] == b[i])
		i++;
	return (((unsigned char)a[i]) - ((unsigned char)b[i]));
}

int	sh_strncmp(const char *a, const char *b, int n)
{
	int	i;

	i = 0;
	if (n <= 0)
		return (0);
	if (!a || !b)
	{
		if (a == b)
			return (0);
		if (a)
			return (1);
		return (-1);
	}
	while (i < n - 1 && a[i] && b[i] && a[i] == b[i])
		i++;
	return (((unsigned char)a[i]) - ((unsigned char)b[i]));
}
