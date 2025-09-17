#include "shell.h"

char *expand_string(t_shell *sh, const char *in)
{
    (void)sh;
    if (!in) return NULL;
    return sh_strdup(in);
}

char **expand_argv(t_shell *sh, char **argv, int argc)
{
    (void)sh;
    (void)argc;
    return argv;
}

char *unquote_string(t_shell *sh, const char *in)
{
    (void)sh;
    if (!in) return NULL;
    return sh_strdup(in);
}

int sh_env_set(t_shell *sh, const char *key, const char *val, int overwrite)
{
    (void)sh;
    (void)key;
    (void)val;
    (void)overwrite;
    return 0;
}

int sh_env_unset(t_shell *sh, const char *key)
{
    (void)sh;
    (void)key;
    return 0;
}