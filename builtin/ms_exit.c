#include "../minishell.h"

static int	is_numeric(char *str)
{
	if (!str || !*str)
		return (0);
	while ((*str >= 9 && *str <= 13) || *str == 32)
		str++;
	if (*str == '-' || *str == '+')
		str++;
	if (!*str)
		return (0);
	while (*str >= '0' && *str <= '9')
		str++;
	while ((*str >= 9 && *str <= 13) || *str == 32)
		str++;
	return (*str == '\0');
}

static int	sh_atoi(char *str)
{
	int	res;
	int	sign;

	res = 0;
	sign = 1;
	while ((*str >= 9 && *str <= 13) || *str == 32)
		str++;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	while (*str >= '0' && *str <= '9')
	{
		res = res * 10 + (*str - '0');
		str++;
	}
	return (res * sign);
}

void	bi_exit(char **argv)
{
	int	code;

	code = get_env()->exit_status;
	if (argv[1])
	{
		if (!is_numeric(argv[1]))
		{
			write(2, "minishell: exit: ", 17);
			write(2, argv[1], sh_strlen(argv[1]));
			write(2, ": numeric argument required\n", 28);
			if (isatty(STDIN_FILENO))
				printf("exit\n");
			gc_exit(2);
		}
		if (argv[2])
		{
			write(2, "minishell: exit: too many arguments\n", 37);
			get_env()->exit_status = 1;
			return ;
		}
		code = sh_atoi(argv[1]) & 0xFF;
	}
	if (isatty(STDIN_FILENO))
		printf("exit\n");
	gc_exit(code);
}
