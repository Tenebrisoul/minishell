/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft_0.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 01:10:38 by btuncer           #+#    #+#             */
/*   Updated: 2025/06/26 18:12:08 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <sys/types.h>

void *alloc(ssize_t size);

bool is_str_empty(char *str)
{
    int counter;
    bool is_empty;
    
    counter = 0;
    is_empty = true;
    while (str[counter] && is_empty)
    {
        if (str[counter] != ' ')
            is_empty = false;
        counter++;
    }
    return (is_empty);
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

void ft_strcpy(char *src, char *dst)
{
	ssize_t counter;

	counter = 0;
	while (counter < len(src))
	{
		dst[counter] = src[counter];
		counter++;
	}
	dst[counter] = '\0';
}

bool ft_strcmp(char *str, char *to_cmp)
{
	int counter;

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

long ft_atol(char *str)
{
	long res;

	res = 0;
	while (*str)
	{
		res = res * 10;
		res = res + (*str - 48);
		str++;
	}

	return (res);
}

long len_digit(long l)
{
	int n;

	n = 0;
	while (l)
	{
		n++;
		l /= 10;
	}
	return (n);
}

long power_of10(long times)
{
	long res;

	res = 1;
	while (times--)
		res *= 10;
	return (res);
}

char *ft_ltoa(long l)
{
	char *res;
	int n;
	int counter;
	
	if (l == 0)
		return ("0");
	n = len_digit(l);
	res = alloc((n + 1) * sizeof(char));
	if (!res)
		return ((void *)0);
	counter = 0;
	while (n > 0)
	{
		res[counter] = l / power_of10(n - 1) + 48;
		counter++;
		l = l % power_of10(n - 1);
		n--;
	}
	res[counter] = '\0';
	return (res);
}
