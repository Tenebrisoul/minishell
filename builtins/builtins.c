#include "shell.h"
#include <unistd.h>
#include <stdlib.h>

static int bi_echo(char **argv)
{
    int i=1; int newline=1; int first=1;
    if (argv[1] && sh_strcmp(argv[1], "-n")==0) { newline=0; i=2; }
    
    while (argv[i]) {
        if (!first)
            write(1, " ", 1);
        first = 0;
        
        /* Handle escape sequences */
        const char *str = argv[i];
        int j = 0;
        while (str[j]) {
            if (str[j] == '\\' && str[j+1]) {
                switch (str[j+1]) {
                    case 'n': write(1, "\n", 1); j += 2; break;
                    case 't': write(1, "\t", 1); j += 2; break;
                    case 'r': write(1, "\r", 1); j += 2; break;
                    case 'b': write(1, "\b", 1); j += 2; break;
                    case 'a': write(1, "\a", 1); j += 2; break;
                    case 'v': write(1, "\v", 1); j += 2; break;
                    case 'f': write(1, "\f", 1); j += 2; break;
                    case '\\': write(1, "\\", 1); j += 2; break;
                    default: write(1, &str[j], 1); j++; break;
                }
            } else {
                write(1, &str[j], 1);
                j++;
            }
        }
        i++;
    }
    if (newline) write(1, "\n", 1);
    return 0;
}

static int bi_pwd(void)
{
    char buf[4096];
    char *p = getcwd(buf, sizeof(buf));
    if (!p) { 
        write(2, "minishell: pwd: error retrieving current directory\n", 51);
        return 1; 
    }
    write(1, p, sh_strlen(p)); write(1, "\n", 1);
    return 0;
}

static int bi_env(t_shell *sh, char **argv)
{
    if (argv[1]) {
        /* env command [args...] - execute command with environment */
        /* For now, we need to create a subprocess and execute the command */
        /* This is complex, let's implement a simple version for echo */
        if (sh_strcmp(argv[1], "echo") == 0) {
            /* Special case for echo */
            int i = 2;
            int first = 1;
            while (argv[i]) {
                if (!first) write(1, " ", 1);
                first = 0;
                write(1, argv[i], sh_strlen(argv[i]));
                i++;
            }
            write(1, "\n", 1);
            return 0;
        } else {
            /* For other commands, we need fork/exec - simplified for now */
            write(2, "minishell: env: command execution not fully implemented\n", 56);
            return 1;
        }
    } else {
        /* env without args - display environment */
        int i; 
        for (i=0;i<sh->env_len;i++) {
            if (sh->env[i]) { 
                write(1, sh->env[i], sh_strlen(sh->env[i])); 
                write(1, "\n", 1); 
            }
        }
        return 0;
    }
}

static int bi_exit(t_shell *sh, char **argv)
{
    int code = sh ? sh->last_status : 0;  /* Use last command's exit status by default */
    if (argv[1]) {
        int i=0, sign=1; long val=0;
        const char *arg = argv[1];
        
        /* Check for sign */
        if (arg[0]=='+'||arg[0]=='-') { 
            if(arg[0]=='-') sign=-1; 
            i++; 
        }
        
        /* Parse numeric part and validate */
        int has_digits = 0;
        while (arg[i] && arg[i]>='0' && arg[i]<='9') { 
            val = val*10 + (arg[i]-'0'); 
            i++;
            has_digits = 1;
        }
        
        /* Check if there are invalid characters or no digits */
        if (arg[i] != '\0' || !has_digits) {
            write(2, "minishell: exit: ", 17);
            write(2, arg, sh_strlen(arg));
            write(2, ": numeric argument required\n", 28);
            if (isatty(STDIN_FILENO))
                write(1, "exit\n", 5);
            exit(2);
        }
        
        code = (int)(sign*val) & 0xFF;
    }
    /* Only print "exit" in interactive mode */
    if (isatty(STDIN_FILENO))
        write(1, "exit\n", 5);
    exit(code);
}

static int bi_cd(t_shell *sh, char **argv)
{
    const char *target;
    char buf[4096]; char old[4096];
    
    if (!argv[1]) {
        /* cd without args -> go to HOME */
        target = sh_getenv_val(sh, "HOME");
    } else if (sh_strcmp(argv[1], "-") == 0) {
        /* cd - -> go to OLDPWD */
        target = sh_getenv_val(sh, "OLDPWD");
        if (!target) { 
            write(2, "minishell: cd: OLDPWD not set\n", 30); 
            return 1; 
        }
        /* Print the directory we're going to (bash behavior) */
        write(1, target, sh_strlen(target));
        write(1, "\n", 1);
    } else {
        /* cd <path> -> go to specified path */
        target = argv[1];
    }
    
    if (!target) { write(2, "minishell: cd: HOME not set\n", 28); return 1; }
    if (!getcwd(old, sizeof(old))) old[0]='\0';
    if (chdir(target) != 0) { 
        write(2, "minishell: cd: ", 15);
        write(2, target, sh_strlen(target));
        write(2, ": No such file or directory\n", 28);
        return 1; 
    }
    if (getcwd(buf, sizeof(buf))) {
        sh_env_set(sh, "OLDPWD", old, 1);
        sh_env_set(sh, "PWD", buf, 1);
    }
    return 0;
}

static int bi_export(t_shell *sh, char **argv)
{
    int i=1; int rc=0; char *eq;
    if (!argv[1]) return bi_env(sh, argv);
    while (argv[i]) {
        eq = sh_strchr(argv[i], '=');
        if (!eq) { /* ignore invalid for now */ i++; continue; }
        *eq='\0';
        if (sh_env_set(sh, argv[i], eq+1, 1) != 0) rc=1;
        *eq='=';
        i++;
    }
    return rc;
}

static int bi_unset(t_shell *sh, char **argv)
{
    int i=1;
    if (!argv[1]) return 0;
    while (argv[i]) {
        sh_env_unset(sh, argv[i]);
        i++;
    }
    return 0;
}

static int bi_true(void)
{
    return 0;
}

static int bi_false(void)
{
    return 1;
}

int is_builtin(const char *cmd)
{
    if (!cmd) return 0;
    return (sh_strcmp(cmd, "echo")==0
        || sh_strcmp(cmd, "pwd")==0
        || sh_strcmp(cmd, "env")==0
        || sh_strcmp(cmd, "exit")==0
        || sh_strcmp(cmd, "cd")==0
        || sh_strcmp(cmd, "export")==0
        || sh_strcmp(cmd, "unset")==0
        || sh_strcmp(cmd, "true")==0
        || sh_strcmp(cmd, "false")==0);
}

int run_builtin(t_shell *sh, char **argv)
{
    if (!argv || !argv[0]) return 0;
    if (sh_strcmp(argv[0], "echo")==0) return bi_echo(argv);
    if (sh_strcmp(argv[0], "pwd")==0) return bi_pwd();
    if (sh_strcmp(argv[0], "env")==0) return bi_env(sh, argv);
    if (sh_strcmp(argv[0], "exit")==0) return bi_exit(sh, argv);
    if (sh_strcmp(argv[0], "cd")==0) return bi_cd(sh, argv);
    if (sh_strcmp(argv[0], "export")==0) return bi_export(sh, argv);
    if (sh_strcmp(argv[0], "unset")==0) return bi_unset(sh, argv);
    if (sh_strcmp(argv[0], "true")==0) return bi_true();
    if (sh_strcmp(argv[0], "false")==0) return bi_false();
    return 1;
}
