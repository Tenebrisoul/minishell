#include "shell.h"
#include "../gc/gc.h"
#include <stdlib.h>

static int c_strlen(const char *s)
{
	int i = 0; if (!s) return 0; while (s[i]) i++; return i;
}

static char *c_strdup(const char *s)
{
	int len = c_strlen(s);
	char *p = (char *)gc_malloc((len + 1) * sizeof(char));
	int i;
	if (!p) return NULL;
	for (i = 0; i < len; ++i) p[i] = s[i];
	p[len] = '\0';
	return p;
}

int sh_init(t_shell *sh, char **envp)
{
	int i;
	if (!sh)
		return -1;
	sh->last_status = 0;
	sh->env_len = 0;
	if (!envp) { sh->env = NULL; return 0; }
	while (envp[sh->env_len]) sh->env_len++;
	sh->env = (char **)gc_malloc((sh->env_len + 1) * sizeof(char *));
	if (!sh->env)
		return -1;
	for (i = 0; i < sh->env_len; ++i)
	{
		sh->env[i] = c_strdup(envp[i]);
		if (!sh->env[i])
		{
			while (--i >= 0) free(sh->env[i]);
			free(sh->env);
			sh->env = NULL;
			return -1;
		}
	}
	sh->env[sh->env_len] = NULL;
	return 0;
}

static int env_find_index(t_shell *sh, const char *key)
{
	int i; int klen = c_strlen(key);
	for (i=0; i<sh->env_len; ++i) {
		if (sh->env[i] && !sh_strncmp(sh->env[i], key, klen) && sh->env[i][klen]=='=')
			return i;
	}
	return -1;
}

int sh_env_set(t_shell *sh, const char *key, const char *val, int overwrite)
{
	int idx; int klen = c_strlen(key); int vlen = c_strlen(val);
	char *entry; int i;
	if (!sh || !key || !val) return -1;
	entry = (char*)gc_malloc(klen + 1 + vlen + 1);
	if (!entry) return -1;
	for (i=0;i<klen;i++) entry[i]=key[i];
	entry[klen]='=';
	for (i=0;i<vlen;i++) entry[klen+1+i]=val[i];
	entry[klen+1+vlen]='\0';
	idx = env_find_index(sh, key);
	if (idx >= 0) {
		if (!overwrite) { return 0; }
		sh->env[idx] = entry;
		return 0;
	}
	char **newenv = (char**)gc_malloc((sh->env_len + 2) * sizeof(char*));
	if (!newenv) { return -1; }
	for (i=0;i<sh->env_len;i++) newenv[i]=sh->env[i];
	newenv[sh->env_len] = entry;
	newenv[sh->env_len+1] = NULL;
	sh->env = newenv;
	sh->env_len++;
	return 0;
}

int sh_env_unset(t_shell *sh, const char *key)
{
	int idx = env_find_index(sh, key);
	int i, j;
	if (!sh || !key) return -1;
	if (idx < 0) return 0;
	for (i=idx, j=idx+1; j<sh->env_len; ++i, ++j)
		sh->env[i] = sh->env[j];
	sh->env_len--;
	sh->env[sh->env_len] = NULL;
	return 0;
}
