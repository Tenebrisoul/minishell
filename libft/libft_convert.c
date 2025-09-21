/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft_convert.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:13:16 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/22 02:31:46 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*handle_zero_case(void)
{
	char	*res;

	res = alloc(2 * sizeof(char));
	res[0] = '0';
	res[1] = '\0';
	return (res);
}

static char	*handle_zero_case_env(void)
{
	char	*res;

	res = env_alloc(2 * sizeof(char));
	res[0] = '0';
	res[1] = '\0';
	return (res);
}

char	*ft_ltoa(long l)
{
	char	*res;
	int		n;
	int		counter;

	if (l == 0)
		return (handle_zero_case());
	n = len_digit(l);
	res = alloc((n + 1) * sizeof(char));
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

char	*ft_ltoa_env(long l)
{
	char	*res;
	int		n;
	int		counter;

	if (l == 0)
		return (handle_zero_case_env());
	n = len_digit(l);
	res = env_alloc((n + 1) * sizeof(char));
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
