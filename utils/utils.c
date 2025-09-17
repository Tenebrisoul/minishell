#include "../core/shell.h"
#include <stdlib.h>

int sh_is_line_empty(const char *s)
{
    size_t i = 0;
    if (!s) return 1;
    while (s[i]) {
        if (s[i] != ' ' && s[i] != '\t' && s[i] != '\n' && s[i] != '\r')
            return 0;
        i++;
    }
    return 1;
}

int sh_strlen(const char *s)
{
	int i;

	i = 0;
	if (!s)
		return (0);
	while (s[i])
		i++;
	return (i);
}

char *sh_strdup(const char *s)
{
	int len;
	char *p;
	int i;

	len = sh_strlen(s);
	p = (char *)alloc((len + 1) * sizeof(char));
	if (!p)
		return (NULL);
	i = 0;
	while (i < len)
	{
		p[i] = s[i];
		i++;
	}
	p[len] = '\0';
	return (p);
}

int sh_strcmp(const char *a, const char *b)
{
	int i;

	i = 0;
	if (!a || !b)
	{
		if (a == b)
			return (0);
		if (a)
			return (1);
		return (-1);
	}
	while (a[i] && b[i] && a[i] == b[i])
		i++;
	return (((unsigned char)a[i]) - ((unsigned char)b[i]));
}

int sh_strncmp(const char *a, const char *b, int n)
{
	int i;

	i = 0;
	if (n <= 0)
		return (0);
	if (!a || !b)
	{
		if (a == b)
			return (0);
		if (a)
			return (1);
		return (-1);
	}
	while (i < n - 1 && a[i] && b[i] && a[i] == b[i])
		i++;
	return (((unsigned char)a[i]) - ((unsigned char)b[i]));
}

char *sh_strchr(const char *s, int c)
{
	int i;

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

char *sh_join_path(const char *dir, const char *file)
{
	int dl;
	int fl;
	int need_slash;
	int len;
	char *p;
	int i;
	int j;

	dl = sh_strlen(dir);
	fl = sh_strlen(file);
	need_slash = (dl > 0 && dir[dl - 1] != '/');
	len = dl + need_slash + fl;
	p = (char *)alloc(len + 1);
	if (!p)
		return (NULL);
	i = 0;
	while (i < dl)
	{
		p[i] = dir[i];
		i++;
	}
	if (need_slash)
		p[i++] = '/';
	j = 0;
	while (j < fl)
	{
		p[i + j] = file[j];
		j++;
	}
	p[len] = '\0';
	return (p);
}

static int count_colon_segments(const char *s)
{
	int count;
	int i;

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

static char *create_segment(const char *s, int start, int len)
{
	char *seg;
	int k;

	seg = (char *)alloc(len + 1);
	if (!seg)
		return (NULL);
	k = 0;
	while (k < len)
	{
		seg[k] = s[start + k];
		k++;
	}
	seg[len] = '\0';
	return (seg);
}

static int process_split_segment(const char *s, char **arr, int *idx, int start, int i)
{
	int len;
	char *seg;

	len = i - start;
	seg = create_segment(s, start, len);
	if (!seg)
	{
		arr[*idx] = NULL;
		sh_free_strarray(arr);
		return (0);
	}
	arr[(*idx)++] = seg;
	return (1);
}

static int split_string_helper(const char *s, char **arr, int count)
{
	int i;
	int start;
	int idx;

	i = 0;
	start = 0;
	idx = 0;
	while (1)
	{
		if (s[i] == ':' || s[i] == '\0')
		{
			if (!process_split_segment(s, arr, &idx, start, i))
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

char **sh_split_colon(const char *s)
{
	int count;
	char **arr;

	if (!s)
		return (NULL);
	count = count_colon_segments(s);
	arr = (char **)alloc((count + 1) * sizeof(char *));
	if (!arr)
		return (NULL);
	if (!split_string_helper(s, arr, count))
		return (NULL);
	return (arr);
}

void sh_free_strarray(char **arr)
{
    (void)arr;
}

const char *sh_getenv_val(t_shell *sh, const char *name)
{
	int nlen;
	int i;
	char *e;

	if (!sh || !name)
		return (NULL);
	nlen = sh_strlen(name);
	i = 0;
	while (i < sh->env_len)
	{
		e = sh->env[i];
		if (sh_strncmp(e, name, nlen) == 0 && e[nlen] == '=')
			return (e + nlen + 1);
		i++;
	}
	return (NULL);
}
