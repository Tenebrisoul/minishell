#include "../minishell.h"

char	*get_cwd(void)
{
	char	cwd[8000];
	char	*response;
	int		counter;

	if (getcwd(cwd, sizeof(cwd)) == NULL)
		return (NULL);
	response = alloc(len(cwd) + 1);
	if (!response)
		return (NULL);
	counter = 0;
	while (cwd[counter])
	{
		response[counter] = cwd[counter];
		counter++;
	}
	response[counter] = '\0';
	return (response);
}
