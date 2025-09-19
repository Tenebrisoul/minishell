/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_path.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:24:23 by root              #+#    #+#             */
/*   Updated: 2025/09/19 15:14:03 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*check_direct_path(const char *file)
{
	if (!file || !*file)
		return (NULL);
	if (sh_strchr(file, '/'))
		return (sh_strdup(file));
	return (NULL);
}

static char	*search_in_dirs(char **dirs, const char *file)
{
	int		i;
	char	*full;

	i = 0;
	while (dirs[i])
	{
		full = sh_join_path(dirs[i], file);
		if (!full)
			break ;
		if (access(full, X_OK) == 0)
		{
			return (full);
		}
		i++;
	}
	return (NULL);
}

char	*find_exec_in_path(const char *file)
{
	const char	*path;
	char		**dirs;
	char		*result;

	result = check_direct_path(file);
	if (result)
		return (result);
	path = sh_getenv_val("PATH");
	if (!path)
		return (NULL);
	dirs = sh_split_colon(path);
	if (!dirs)
		return (NULL);
	result = search_in_dirs(dirs, file);
	return (result);
}
