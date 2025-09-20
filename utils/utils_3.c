/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_3.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:25:28 by root              #+#    #+#             */
/*   Updated: 2025/09/20 21:20:23 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	count_colon_segments(const char *s)
{
	int	count;
	int	i;

	count = 1;
	i = 0;
	while (s[i])
	{
		if (s[i] == ':')
			count++;
		i++;
	}
	return (count);
}

static char	*create_segment(const char *s, int start, int len)
{
	char	*seg;
	int		k;

	seg = (char *)alloc(len + 1);
	k = 0;
	while (k < len)
	{
		seg[k] = s[start + k];
		k++;
	}
	seg[len] = '\0';
	return (seg);
}

static int	process_split_segment(const char *s, char **arr, int *idx,
		int start)
{
	int		len;
	char	*seg;
	int		i;

	i = start;
	while (s[i] && s[i] != ':')
		i++;
	len = i - start;
	seg = create_segment(s, start, len);
	if (!seg)
	{
		arr[*idx] = NULL;
		return (0);
	}
	arr[(*idx)++] = seg;
	return (1);
}

static int	split_string_helper(const char *s, char **arr)
{
	int	i;
	int	start;
	int	idx;

	i = 0;
	start = 0;
	idx = 0;
	while (1)
	{
		if (s[i] == ':' || s[i] == '\0')
		{
			if (!process_split_segment(s, arr, &idx, start))
				return (0);
			if (s[i] == '\0')
				break ;
			start = i + 1;
		}
		i++;
	}
	arr[idx] = NULL;
	return (1);
}

char	**sh_split_colon(const char *s)
{
	int		count;
	char	**arr;

	if (!s)
		return (NULL);
	count = count_colon_segments(s);
	arr = (char **)alloc((count + 1) * sizeof(char *));
	if (!split_string_helper(s, arr))
		return (NULL);
	return (arr);
}
