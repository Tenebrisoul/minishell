#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

bool	is_str_empty(char *str)
{
	int		counter;
	bool	is_empty;

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
