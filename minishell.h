/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 01:15:00 by root              #+#    #+#             */
/*   Updated: 2025/09/21 05:28:39 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

/* INCLUDES */

# include <dirent.h>
# include <errno.h>
# include <fcntl.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdbool.h>
# include <stddef.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

/* DEFINES */

# define STATE_NORMAL 0
# define STATE_INTERRUPT 1
# define STATE_HEREDOC 2
# define STATE_COMMAND 4

# define GET false
# define INIT true
# define GC_GC 1
# define ENV_GC 2
# define GET_GC 0
# define RESET_GC 1
/* ENUMS */

typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_SEMICOLON,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_REDIR_APPEND,
	TOKEN_HEREDOC,
}								t_token_type;

typedef enum e_node_type
{
	NODE_COMMAND,
	NODE_PIPELINE,
	NODE_SEQUENCE,
}								t_node_type;

typedef enum e_redir_type
{
	REDIR_IN,
	REDIR_OUT,
	REDIR_APPEND,
	REDIR_HEREDOC
}								t_redir_type;

/* STRUCTS */

typedef struct s_token
{
	t_token_type				type;
	char						*value;
	struct s_token				*next;
}								t_token;

typedef struct s_lexer
{
	char						*input;
	int							position;
	int							length;
	char						current_char;
}								t_lexer;

typedef struct s_redirect
{
	t_redir_type				type;
	char						*filename;
	int							quoted_delimiter;
	struct s_redirect			*next;
}								t_redirect;

typedef struct s_command
{
	char						**args;
	int							argc;
	t_redirect					*redirects;
}								t_command;

typedef struct s_ast_node
{
	t_node_type					type;
	union
	{
		t_command				*command;
		struct
		{
			struct s_ast_node	*left;
			struct s_ast_node	*right;
		} s_binary;
	} u_data;
}								t_ast_node;

typedef struct s_parser
{
	t_token						*tokens;
	t_token						*current;
}								t_parser;

typedef struct s_env_item
{
	char						*key;
	char						*value;
	struct s_env_item			*next;
}								t_env_item;

typedef struct s_env
{
	t_env_item					*first_node;
	int							exit_status;
}								t_env;

typedef struct s_trash
{
	void						*mem;
	struct s_trash				*next;
}								t_trash;

typedef struct s_gc
{
	t_trash						*first_node;
	t_trash						*gc_mark;
}								t_gc;

typedef struct s_gc_node
{
	void						*ptr;
	size_t						size;
	bool						marked;
	struct s_gc_node			*next;
}								t_gc_node;

typedef struct s_shell
{
	char						**env;
	int							env_len;
	int							last_status;
}								t_shell;

typedef struct s_expander
{
	char						*prompt;
	char						**queue;
	int							marker;
	int							queue_marker;
}								t_expander;

/* FUNCTION DECLARATIONS */

/* MAIN */
int								main(void);
void							*init_env(void);
char							*handle_input(char *prompt);
int								is_getline_allocated(void);
void							setup_tty(int *saved_in, int *saved_out);
void							restore_tty(int saved_in, int saved_out);

/* LEXER */
t_token							*lexer(char *input);
void							advance_char(t_lexer *lexer);
void							cleanup_tokens(t_token *tokens);
char							peek_char(t_lexer *lexer, int offset);
t_token							*create_token(t_token_type type, char *value);
void							add_token_to_list(t_token **head,
									t_token *new_token);
bool							is_quote(char c);
bool							is_operator(char c);
bool							is_word_char(char c);
bool							is_whitespace(char c);
bool							is_double_operator(t_lexer *lexer);
t_token							*read_word(t_lexer *lexer);
t_token							*read_operator(t_lexer *lexer);

/* PARSER */
t_ast_node						*parser(t_token *tokens);
t_ast_node						*create_ast_node(t_node_type type);
void							advance_token(t_parser *parser);
void							cleanup_redirections(t_redirect *redirects);
void							cleanup_command(t_command *cmd);
void							cleanup_ast(t_ast_node *ast);
bool							is_redirect_token(t_token_type type);
t_ast_node						*parse_ast_node(t_parser *parser);
t_command						*parse_command(t_parser *parser);
t_redirect						*parse_redirections(t_parser *parser);
t_ast_node						*parse_primary(t_parser *parser);
t_ast_node						*parse_pipeline(t_parser *parser);
t_ast_node						*parse_sequence(t_parser *parser);
t_ast_node						*create_pipeline_node(t_ast_node *left,
									t_ast_node *right);
t_ast_node						*create_sequence_node(t_ast_node *left,
									t_ast_node *right);
t_redirect						*process_redirect_token(t_parser *parser);
t_redirect						*create_redirect(t_parser *parser,
									t_token_type redir_token);

/* EXECUTOR */
int								exec_ast(const t_ast_node *ast);
int								apply_redirs(const t_redirect *r);
char							*find_exec_in_path(const char *file);
void							expand_args(const t_command *cmd);
void							expand_redirects(const t_command *cmd);
int								exec_external_command(const t_command *cmd,
									char **argv);
int								exec_child_process(const t_command *cmd,
									char **argv);
int								wait_for_child(pid_t pid);
int								exec_builtin_with_redir(const t_command *cmd,
									char **argv);
int								exec_command(const t_command *cmd);
int								exec_sequence(const t_ast_node *left,
									const t_ast_node *right);
int								exec_pipeline(const t_ast_node *left,
									const t_ast_node *right);
int								write_heredoc_content(int pipefd[2],
									const char *content);

/* BUILTINS */
int								is_builtin(const char *cmd);
int								run_builtin(char **argv);
char							*get_cwd(void);
void							fill_env_with_value(char *env_str,
									t_env_item *node, int key_len);
void							fill_env_item(char **sorted_env,
									t_env_item *node, int *i);
void							bubble_sort_env(char **env_array, int count);
int								bi_echo(char **argv);
int								bi_pwd(void);
int								bi_env(char **argv);
void							bi_exit(char **argv);
int								bi_cd(char **argv);
int								bi_export(char **argv);
int								bi_unset(char **argv);
char							**get_sorted_env(void);

/* ENVIRONMENT */
t_env_item						*new_env_item(char *key, char *val);
t_env_item						*is_env_item_exists(char *key);
t_env_item						*get_env_item(char *key);
t_env_item						*add_env_item(t_env_item *item);
void							unset_env_item(char *key);
void							print_env(void);
char							**get_env_array(void);
t_env							*get_env(void);
t_env							*new_env(void);

/* SHELL CORE */
int								shell_run(void);
int								process_line(char *line);
char							*read_heredoc_input(const char *delimiter);

/* HEREDOC UTILS */
void							copy_content(char *dest, char *src, int len);
char							*resize_content(char *content, int *cap,
									int len, int line_len);
void							add_line_to_content(char **content,
									int *content_len, char *expanded,
									int line_len);
int								check_delimiter(char *line,
									const char *delimiter, int delim_len);
char							*get_heredoc_line(void);
char							*process_heredoc_line_raw(char *content,
									int *content_len, int *content_cap,
									char *line);
char							*expand_heredoc_content(char *content,
									int quoted);
char							*safe_heredoc(char *prompt);
void							print_heredoc_warning(char *delimiter);

/* EXPANSION */
char							**expand_argv(char **argv, int argc);
char							*find_variable_in_string(char *str,
									char *var_name);
void							replace_single_variable(char *str,
									char *var_name, char *value);
void							apply_all_replacements(char *result);
char							*replace_all_variables(void);
char							*get_variable_value(char *key);
char							*expand(char *prompt);
int								calculate_result_size(void);
int								calculate_final_length(void);
int								valid_dollar_count(char *str);
t_expander						*get_expander(bool value);
char							*remove_outer_quotes(char *str);
int								has_quotes(const char *str);
void							queue_expandables(void);
int								quote_status(char *str, int index);
bool							is_char(char c);
bool							is_number(char c);
char							*process_escapes(char *str);

/* ENVIRONMENT UTILS */
int								sh_env_set(const char *key, const char *val,
									int overwrite);
int								sh_env_unset(const char *key);
const char						*sh_getenv_val(const char *name);

/* SIGNALS */
void							init_signals(void);
void							sh_signal_set_state(int state_type, int value);
int								sh_signal_interrupted(void);
int								sh_signal_should_exit(void);
void							sh_signal_reset(void);

/* UTILS */
int								sh_is_line_empty(const char *s);
int								sh_strlen(const char *s);
char							*sh_strdup(const char *s);
char							*sh_env_strdup(const char *s);
int								sh_strcmp(const char *a, const char *b);
int								sh_strncmp(const char *a, const char *b, int n);
char							*sh_strchr(const char *s, int c);
char							*sh_join_path(const char *dir,
									const char *file);
char							**sh_split_colon(const char *s);
void							sh_free_strarray(char **arr);

/* LIBFT */
size_t							ft_strlen(const char *str);
bool							is_str_empty(char *str);
bool							in(char *str, char c);
ssize_t							len(const char *str);
void							ft_strcpy(char *src, char *dst);
void							ft_strncpy(char *src, char *dst, int n);
bool							ft_strcmp(char *str, char *to_cmp);
long							ft_atol(char *str);
char							*ft_ltoa(long l);
char							*ft_ltoa_env(long l);
char							**ft_split(char const *s, char c);
long							power_of10(long times);
long							len_digit(long l);

/* GARBAGE COLLECTOR */
void							*alloc(ssize_t size);
void							*env_alloc(ssize_t size);
t_gc							*get_gc(int option);
t_gc							*get_env_gc(int option);
t_gc							*new_gc(void);
t_trash							*new_trash(void *mem);
void							insert_to_gc(t_trash *new_trash, int gcd);
void							dump_gc(void);
void							dump_env_gc(void);
void							gc_exit(int code);

#endif