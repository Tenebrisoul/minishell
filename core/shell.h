/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell <minishell@student.42>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 00:00:00 by minishell         #+#    #+#             */
/*   Updated: 2025/09/11 00:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHELL_H
# define SHELL_H

# include <stdbool.h>
# include <stddef.h>
# include <stdio.h>

# include "../lexer/lexer.h"
# include "../parser/parser.h"
# include "../gc/gc.h"

typedef struct s_shell
{
	char **env;
	int    env_len;
	int    last_status;
}   t_shell; // note

int  shell_run(t_shell *sh);

int  sh_is_line_empty(const char *s);
int  sh_strlen(const char *s);
char *sh_strdup(const char *s);
int  sh_strcmp(const char *a, const char *b);
int  sh_strncmp(const char *a, const char *b, int n);
char *sh_strchr(const char *s, int c);
char *sh_join_path(const char *dir, const char *file);
char **sh_split_colon(const char *s);
void sh_free_strarray(char **arr);
const char *sh_getenv_val(t_shell *sh, const char *name);
int   sh_env_set(t_shell *sh, const char *key, const char *val, int overwrite);
int   sh_env_unset(t_shell *sh, const char *key);

int   sh_init(t_shell *sh);
void  sh_destroy(t_shell *sh);

struct s_ast_node;
int   exec_ast(t_shell *sh, const struct s_ast_node *ast);

int  is_builtin(const char *cmd);
int  run_builtin(t_shell *sh, char **argv);

char *expand_string(t_shell *sh, const char *in);
char **expand_argv(t_shell *sh, char **argv, int argc);
char *unquote_string(t_shell *sh, const char *in);

void sh_setup_signals(void);

// Signal state management
#define STATE_NORMAL	0
#define STATE_INTERRUPT	1
#define STATE_HEREDOC	2
#define STATE_COMMAND	4

void sh_signal_set_state(int state_type, int value);
int  sh_signal_interrupted(void);
void sh_signal_reset(void);
void init_signals(void);

// Heredoc support functions
int  handle_heredoc(t_shell *sh, const char *delimiter);
char *read_heredoc_input(t_shell *sh, const char *delimiter);

#endif
