#include "../minishell.h"
#include <dirent.h>
#include <sys/stat.h>

t_wildcard *wildcard_init(void)
{
    t_wildcard *wc;

    wc = alloc(sizeof(t_wildcard));
    if (!wc)
        return (NULL);
    wc->matches = alloc(sizeof(char *) * 16);
    if (!wc->matches)
        return (NULL);
    wc->count = 0;
    wc->capacity = 16;
    return (wc);
}

void wildcard_free(t_wildcard *wc)
{
    int i;

    if (!wc)
        return;
    i = 0;
    while (i < wc->count)
    {
        free(wc->matches[i]);
        i++;
    }
    free(wc->matches);
    free(wc);
}

void wildcard_add_match(t_wildcard *wc, const char *match)
{
    char **new_matches;
    int i;

    if (!wc || !match)
        return;

    if (wc->count >= wc->capacity)
    {
        wc->capacity *= 2;
        new_matches = alloc(sizeof(char *) * wc->capacity);
        if (!new_matches)
            return;
        i = 0;
        while (i < wc->count)
        {
            new_matches[i] = wc->matches[i];
            i++;
        }
        wc->matches = new_matches;
    }

    wc->matches[wc->count] = ft_strdup(match);
    wc->count++;
}

bool wildcard_match(const char *pattern, const char *string)
{
    const char *p = pattern;
    const char *s = string;
    const char *star_p = NULL;
    const char *star_s = NULL;

    while (*s)
    {
        if (*p == '*')
        {
            star_p = p++;
            star_s = s;
        }
        else if (*p == *s)
        {
            p++;
            s++;
        }
        else if (star_p)
        {
            p = star_p + 1;
            s = ++star_s;
        }
        else
        {
            return (false);
        }
    }

    while (*p == '*')
        p++;

    return (*p == '\0');
}

static int compare_strings(const void *a, const void *b)
{
    return (sh_strcmp(*(char **)a, *(char **)b));
}

char **wildcard_expand(const char *pattern)
{
    DIR *dir;
    struct dirent *entry;
    t_wildcard *wc;
    char **result;
    int i;

    // Check if pattern contains wildcard
    if (!sh_strchr(pattern, '*'))
    {
        result = alloc(sizeof(char *) * 2);
        if (!result)
            return (NULL);
        result[0] = ft_strdup(pattern);
        result[1] = NULL;
        return (result);
    }

    wc = wildcard_init();
    if (!wc)
        return (NULL);

    dir = opendir(".");
    if (!dir)
    {
        wildcard_free(wc);
        return (NULL);
    }

    while ((entry = readdir(dir)) != NULL)
    {
        // Skip hidden files unless pattern starts with '.'
        if (entry->d_name[0] == '.' && pattern[0] != '.')
            continue;

        if (wildcard_match(pattern, entry->d_name))
            wildcard_add_match(wc, entry->d_name);
    }

    closedir(dir);

    if (wc->count == 0)
    {
        // No matches found, return original pattern
        result = alloc(sizeof(char *) * 2);
        if (!result)
        {
            wildcard_free(wc);
            return (NULL);
        }
        result[0] = ft_strdup(pattern);
        result[1] = NULL;
        wildcard_free(wc);
        return (result);
    }

    // Sort matches alphabetically
    qsort(wc->matches, wc->count, sizeof(char *), compare_strings);

    // Create result array
    result = alloc(sizeof(char *) * (wc->count + 1));
    if (!result)
    {
        wildcard_free(wc);
        return (NULL);
    }

    i = 0;
    while (i < wc->count)
    {
        result[i] = ft_strdup(wc->matches[i]);
        i++;
    }
    result[i] = NULL;

    wildcard_free(wc);
    return (result);
}