#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>
#include "../minishell.h" 

long	ft_atol(char *str)
{
	long	res;

	res = 0;
	while (*str)
	{
		res = res * 10;
		res = res + (*str - 48);
		str++;
	}
	return (res);
}

long	len_digit(long l)
{
	int	n;

	n = 0;
	while (l)
	{
		n++;
		l /= 10;
	}
	return (n);
}

long	power_of10(long times)
{
	long	res;

	res = 1;
	while (times--)
		res *= 10;
	return (res);
}
