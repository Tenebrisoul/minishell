/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft_string.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 21:30:00 by root              #+#    #+#             */
/*   Updated: 2025/09/19 21:30:00 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

ssize_t	len(char *str)
{
	ssize_t	counter;

	if (!str)
		return (0);
	counter = 0;
	while (str[counter])
		counter++;
	return (counter);
}

void	ft_strcpy(char *src, char *dst)
{
	ssize_t	counter;

	counter = 0;
	while (counter < len(src))
	{
		dst[counter] = src[counter];
		counter++;
	}
	dst[counter] = '\0';
}

void	ft_strncpy(char *src, char *dst, int n)
{
	ssize_t	counter;

	counter = 0;
	while (counter < len(src) && counter < n)
	{
		dst[counter] = src[counter];
		counter++;
	}
	dst[counter] = '\0';
}

bool	ft_strcmp(char *str, char *to_cmp)
{
	int	counter;

	counter = 0;
	if (len(str) != len(to_cmp))
		return (false);
	while (str[counter])
	{
		if (str[counter] != to_cmp[counter])
			return (false);
		counter++;
	}
	return (true);
}

bool	in(char *str, char c)
{
	ssize_t	counter;

	counter = 0;
	while (str[counter])
	{
		if (str[counter] == c)
			return (true);
		counter++;
	}
	return (false);
}
