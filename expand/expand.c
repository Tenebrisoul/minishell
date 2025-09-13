/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell <minishell@student.42>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 00:00:00 by minishell         #+#    #+#             */
/*   Updated: 2025/09/11 00:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <stdlib.h>
#include <unistd.h>

static int is_name_start(char c){ return (c=='_'||(c>='A'&&c<='Z')||(c>='a'&&c<='z')); }
static int is_name_char(char c){ return is_name_start(c)||(c>='0'&&c<='9'); }

char *expand_string(t_shell *sh, const char *in)
{
    int i=0, len=0, cap=64; 
    char *out = (char*)gc_malloc(cap); 
    int in_s=0, in_d=0; 
    if(!out) return NULL;
    while (in[i]) {
        char c = in[i];
        if (c=='\'' && !in_d) { 
            /* Single quote - only toggle if not in double quotes */
            in_s = !in_s; 
            /* Don't include the quote in output - it's a delimiter */
            i++; 
            continue; 
        }
        if (c=='"' && !in_s) { 
            /* Double quote - only toggle if not in single quotes */
            in_d = !in_d; 
            /* Don't include the quote in output - it's a delimiter */
            i++; 
            continue; 
        }
        
        /* Handle quotes that should be preserved (inside opposite quote type) */
        if ((c=='\'' && in_d) || (c=='"' && in_s)) {
            /* Quote inside opposite quote type - preserve it */
            if (len+2>cap){ int k; cap*=2; char *tmp=(char*)malloc(cap); if(!tmp){free(out);return NULL;} for(k=0;k<len;k++) tmp[k]=out[k]; free(out); out=tmp; }
            out[len++]=c; 
            i++;
            continue;
        }
        
        /* Handle special characters inside quotes */
        if (in_s) {
            /* Inside single quotes - everything is literal except the closing quote */
            if (len+2>cap){ int k; cap*=2; char *tmp=(char*)malloc(cap); if(!tmp){free(out);return NULL;} for(k=0;k<len;k++) tmp[k]=out[k]; free(out); out=tmp; }
            out[len++]=c; i++;
            continue;
        }
        
        if (c=='~' && !in_s && !in_d && (i==0 || in[i-1]==' ')) {
            /* Tilde expansion: ~ -> HOME or ~user -> user's home */
            if (in[i+1]=='\0' || in[i+1]==' ' || in[i+1]=='/') {
                /* Simple ~ case */
                const char *home = sh_getenv_val(sh, "HOME");
                if (home) {
                    int hlen = sh_strlen(home); int k;
                    while (len+hlen+1>cap){ cap*=2; char *tmp=(char*)malloc(cap); if(!tmp){free(out);return NULL;} for(k=0;k<len;k++) tmp[k]=out[k]; free(out); out=tmp; }
                    for (k=0;k<hlen;k++) out[len++]=home[k];
                    i++; continue;
                }
            } else if (is_name_start(in[i+1])) {
                /* ~username case */
                int j = i+1;
                while (is_name_char(in[j])) j++;
                int ulen = j - (i+1);
                char username[256];
                int k;
                
                if (ulen > 255) ulen = 255;
                for (k = 0; k < ulen; k++) username[k] = in[i+1+k];
                username[ulen] = '\0';
                
                /* For now, only support "root" user - in real implementation would use getpwnam() */
                if (sh_strcmp(username, "root") == 0) {
                    const char *root_home = "/root";
                    int hlen = sh_strlen(root_home);
                    while (len+hlen+1>cap){ cap*=2; char *tmp=(char*)malloc(cap); if(!tmp){free(out);return NULL;} for(k=0;k<len;k++) tmp[k]=out[k]; free(out); out=tmp; }
                    for (k=0;k<hlen;k++) out[len++]=root_home[k];
                    i = j; continue;
                }
                /* If user not found, leave as is (bash behavior for unknown users) */
            }
        }
        if (c=='$' && !in_s) {
            if (in[i+1]=='?') {
                int st = sh?sh->last_status:0; char buf[16]; int b=15; buf[15]='\0';
                if (st==0){ buf[--b]='0'; }
                else { int v=st; if (v<0) v=-v; while (v>0 && b>0){ buf[--b] = '0'+(v%10); v/=10; } }
                const char *val=&buf[b]; int vlen=sh_strlen(val); int k;
                while (len+vlen+1>cap){ cap*=2; char *tmp=(char*)gc_malloc(cap); if(!tmp){return NULL;} for(k=0;k<len;k++) tmp[k]=out[k]; out=tmp; }
                for (k=0;k<vlen;k++) out[len++]=val[k];
                i+=2; continue;
            }
            if (in[i+1]=='$') {
                /* $$ - Process ID expansion */
                int pid = getpid(); char buf[16]; int b=15; buf[15]='\0';
                if (pid==0){ buf[--b]='0'; }
                else { int v=pid; while (v>0 && b>0){ buf[--b] = '0'+(v%10); v/=10; } }
                const char *val=&buf[b]; int vlen=sh_strlen(val); int k;
                while (len+vlen+1>cap){ cap*=2; char *tmp=(char*)gc_malloc(cap); if(!tmp){return NULL;} for(k=0;k<len;k++) tmp[k]=out[k]; out=tmp; }
                for (k=0;k<vlen;k++) out[len++]=val[k];
                i+=2; continue;
            }
            if (in[i+1]=='{') {
                /* ${VAR} - Brace expansion */
                int j=i+2; while (in[j] && in[j]!='}') j++;
                if (in[j]=='}') {
                    int nlen=j-(i+2); char key[256]; int k;
                    if (nlen>255) nlen=255;
                    for (k=0;k<nlen;k++) key[k]=in[i+2+k];
                    key[nlen]='\0';
                    const char *val = sh_getenv_val(sh, key);
                    if (val){
                        int vlen=sh_strlen(val);
                        while (len+vlen+1>cap){ cap*=2; char *tmp=(char*)gc_malloc(cap); if(!tmp){return NULL;} for(k=0;k<len;k++) tmp[k]=out[k]; out=tmp; }
                        for (k=0;k<vlen;k++) out[len++]=val[k];
                    }
                    i=j+1; continue;
                }
                /* If no closing brace, treat as literal */
            }
            if (is_name_start(in[i+1])) {
                int j=i+1; while (is_name_char(in[j])) j++;
                int nlen=j-(i+1); char key[256]; int k;
                if (nlen>255) nlen=255;
                for (k=0;k<nlen;k++) key[k]=in[i+1+k];
                key[nlen]='\0';
                const char *val = sh_getenv_val(sh, key);
                if (val){
                    int vlen=sh_strlen(val);
                    while (len+vlen+1>cap){ cap*=2; char *tmp=(char*)gc_malloc(cap); if(!tmp){return NULL;} for(k=0;k<len;k++) tmp[k]=out[k]; out=tmp; }
                    for (k=0;k<vlen;k++) out[len++]=val[k];
                }
                i=j; continue;
            }
        }
        if (len+2>cap){ int k; cap*=2; char *tmp=(char*)gc_malloc(cap); if(!tmp){return NULL;} for(k=0;k<len;k++) tmp[k]=out[k]; out=tmp; }
        out[len++]=c; i++;
    }
    out[len]='\0';
    return out;
}

char **expand_argv(t_shell *sh, char **argv, int argc)
{
    int i; char **out = (char**)gc_malloc((argc+1)*sizeof(char*));
    if (!out) return NULL;
    for (i=0;i<argc;i++){
        out[i]=expand_string(sh, argv[i]);
        if (!out[i]){ return NULL; } /* GC will handle cleanup */
    }
    out[argc]=NULL; return out;
}

char *unquote_string(t_shell *sh, const char *in)
{
    (void)sh; /* Unused parameter - kept for compatibility */
    int i=0, len=0, cap=64; 
    char *out = (char*)gc_malloc(cap); 
    int in_s=0, in_d=0; 
    if(!out) return NULL;
    
    while (in && in[i]){
        char c=in[i];
        
        if (c=='\'' && !in_d){ 
            /* Single quote - toggle single quote mode only if not in double quotes */
            in_s=!in_s; 
            i++; 
            continue; 
        }
        
        if (c=='"' && !in_s){ 
            /* Double quote - toggle double quote mode only if not in single quotes */
            in_d=!in_d; 
            i++; 
            continue; 
        }
        
        /* Handle quotes that should be preserved (inside opposite quote type) */
        if ((c=='\'' && in_d) || (c=='"' && in_s)) {
            /* Quote inside opposite quote type - preserve it */
            if (len+2>cap){ 
                int k; 
                cap*=2; 
                char *tmp=(char*)gc_malloc(cap); 
                if(!tmp){return NULL;} 
                for(k=0;k<len;k++) tmp[k]=out[k]; 
                out=tmp; 
            }
            out[len++]=c; 
            i++;
            continue;
        }
        
        /* Regular character - add to output */
        if (len+2>cap){ 
            int k; 
            cap*=2; 
            char *tmp=(char*)malloc(cap); 
            if(!tmp){free(out);return NULL;} 
            for(k=0;k<len;k++) tmp[k]=out[k]; 
            free(out); 
            out=tmp; 
        }
        out[len++]=c; 
        i++;
    }
    out[len]='\0';
    return out;
}
