/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft_string.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:13:23 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/22 02:09:16 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
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
