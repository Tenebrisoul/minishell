/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft_convert.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 21:30:00 by root              #+#    #+#             */
/*   Updated: 2025/09/20 21:21:03 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

void	*alloc(ssize_t size);
long	len_digit(long l);
long	power_of10(long times);

static char	*handle_zero_case(void)
{
	char	*res;

	res = alloc(2 * sizeof(char));
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
