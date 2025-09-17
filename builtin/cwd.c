#include "minishell.h"

char *get_cwd()
{
	char cwd[8000];
	char *response;
	int counter;
	
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

void pwd()
{
	char *cwd = NULL;

	cwd = get_cwd();
	if (!cwd)
		write(1, "pwd: couldn't fetch cwd.\n", 25);
	else
		write(1, cwd, len(cwd));
}

static char *get_prefix()
{
	char *cwd = NULL;
	char *cwd_ptr = NULL;
	char *prefix = NULL;
	int counter;
	
	cwd = get_cwd();
	if (!cwd)
		return (NULL);
	cwd_ptr = cwd + len(cwd) - 1;
	while (*cwd_ptr)
	{
		if (*cwd_ptr == '/')
			break;
		cwd_ptr--;
	}
	prefix = alloc(len(cwd_ptr) + 1);
	if (!prefix)
		return (NULL);
	counter = 0;
	while (*cwd_ptr)
	{
		prefix[counter] = *cwd_ptr;
		counter++;
		cwd_ptr++;
	}
	prefix[counter] = '\0';
	return (prefix);
}

void print_prefix()
{
	char *prefix;

	prefix = get_prefix();
	if (!prefix)
		return ;
	write(1, prefix, strlen(prefix));
	write(1, "/.", 2);
}
