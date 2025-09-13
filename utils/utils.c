#include "shell.h"
#include "../gc/gc.h"
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
    int i = 0; if (!s) return 0; while (s[i]) i++; return i;
}

char *sh_strdup(const char *s)
{
    int len = sh_strlen(s);
    char *p = (char *)gc_malloc((len + 1) * sizeof(char));
    int i;
    if (!p) return NULL;
    for (i = 0; i < len; ++i) p[i] = s[i];
    p[len] = '\0';
    return p;
}

int sh_strcmp(const char *a, const char *b)
{
    int i = 0;
    if (!a || !b) return (a==b)?0:(a?1:-1);
    while (a[i] && b[i] && a[i]==b[i]) i++;
    return ((unsigned char)a[i]) - ((unsigned char)b[i]);
}

int sh_strncmp(const char *a, const char *b, int n)
{
    int i = 0; if (n<=0) return 0;
    if (!a || !b) return (a==b)?0:(a?1:-1);
    while (i<n-1 && a[i] && b[i] && a[i]==b[i]) i++;
    return ((unsigned char)a[i]) - ((unsigned char)b[i]);
}

char *sh_strchr(const char *s, int c)
{
    int i = 0; if (!s) return NULL;
    while (s[i]) { if (s[i]==(char)c) return (char*)(s+i); i++; }
    if (c=='\0') return (char*)(s+i);
    return NULL;
}

char *sh_join_path(const char *dir, const char *file)
{
    int dl = sh_strlen(dir), fl = sh_strlen(file);
    int need_slash = (dl>0 && dir[dl-1] != '/');
    int len = dl + need_slash + fl;
    char *p = (char *)gc_malloc(len+1);
    int i=0, j=0;
    if (!p) return NULL;
    for (i=0;i<dl;i++) p[i]=dir[i];
    if (need_slash) p[i++] = '/';
    for (j=0;j<fl;j++) p[i+j]=file[j];
    p[len]='\0';
    return p;
}

char **sh_split_colon(const char *s)
{
    int count=1, i=0, start=0, idx=0;
    char **arr;
    if (!s) return NULL;
    while (s[i]) { if (s[i]==':') count++; i++; }
    arr = (char**)gc_malloc((count+1)*sizeof(char*));
    if (!arr) return NULL;
    i=0; while (1) {
        if (s[i]==':' || s[i]=='\0') {
            int len = i-start;
            char *seg = (char*)gc_malloc(len+1);
            int k; if (!seg) { arr[idx] = NULL; sh_free_strarray(arr); return NULL; }
            for (k=0;k<len;k++) seg[k]=s[start+k];
            seg[len]='\0';
            arr[idx++]=seg;
            if (s[i]=='\0') break;
            start=i+1;
        }
        i++;
    }
    arr[idx]=NULL;
    return arr;
}

void sh_free_strarray(char **arr)
{
    (void)arr;
}

const char *sh_getenv_val(t_shell *sh, const char *name)
{
    int nlen, i;
    if (!sh || !name) return NULL;
    nlen = sh_strlen(name);
    for (i=0; i<sh->env_len; ++i) {
        char *e = sh->env[i];
        if (sh_strncmp(e, name, nlen)==0 && e[nlen]=='=')
            return e+nlen+1;
    }
    return NULL;
}
