#include "minishell.h"

char *expand_string(const char *in)
{
    if (!in) return NULL;
    return sh_strdup(in);
}

char **expand_argv(char **argv, int argc)
{
    (void)argc;
    return argv;
}

char *unquote_string(const char *in)
{
    if (!in) return NULL;
    return sh_strdup(in);
}

int sh_env_set(const char *key, const char *val, int overwrite)
{
    t_env_item *new_item;
    t_env_item *existing;
    
    if (!key || !val) return -1;
    
    existing = get_env_item((char *)key);
    if (existing && !overwrite) return 0;
    
    if (existing) {
        existing->value = sh_strdup(val);
        return 0;
    }
    
    new_item = new_env_item((char *)key, (char *)val);
    add_env_item(new_item);
    return 0;
}

int sh_env_unset(const char *key)
{
    if (!key) return -1;
    unset_env_item((char *)key);
    return 0;
}

const char *sh_getenv_val(const char *name)
{
    t_env_item *item;
    
    if (!name) return NULL;
    item = get_env_item((char *)name);
    if (!item) return NULL;
    return item->value;
}