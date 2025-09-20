/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:25:28 by root              #+#    #+#             */
/*   Updated: 2025/09/20 21:20:27 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*sh_strchr(const char *s, int c)
{
	int	i;

	i = 0;
	if (!s)
		return (NULL);
	while (s[i])
	{
		if (s[i] == (char)c)
			return ((char *)(s + i));
		i++;
	}
	if (c == '\0')
		return ((char *)(s + i));
	return (NULL);
}

static void	copy_dir_to_path(char *p, const char *dir, int dl)
{
	int	i;

	i = 0;
	while (i < dl)
	{
		p[i] = dir[i];
		i++;
	}
}

static void	copy_file_to_path(char *p, const char *file, int pos, int fl)
{
	int	j;

	j = 0;
	while (j < fl)
	{
		p[pos + j] = file[j];
		j++;
	}
}

char	*sh_join_path(const char *dir, const char *file)
{
	int		dl;
	int		fl;
	int		need_slash;
	char	*p;

	dl = sh_strlen(dir);
	fl = sh_strlen(file);
	need_slash = (dl > 0 && dir[dl - 1] != '/');
	p = (char *)alloc(dl + need_slash + fl + 1);
	copy_dir_to_path(p, dir, dl);
	if (need_slash)
		p[dl] = '/';
	copy_file_to_path(p, file, dl + need_slash, fl);
	p[dl + need_slash + fl] = '\0';
	return (p);
}

const char	*sh_getenv_val(const char *name)
{
	t_env_item	*item;

	if (!name)
		return (NULL);
	item = get_env_item((char *)name);
	if (item)
		return (item->value);
	return (NULL);
}
