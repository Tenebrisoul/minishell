#ifndef MINISHELL_H
# define MINISHELL_H

/* ================================= INCLUDES ================================ */

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <stdbool.h>
# include <stddef.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <signal.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <dirent.h>
# include <sys/stat.h>
# include <errno.h>

/* ================================= DEFINES ================================= */

# define STATE_NORMAL		0
# define STATE_INTERRUPT	1
# define STATE_HEREDOC		2  
# define STATE_COMMAND      4

# define GET	false
# define INIT	true

/* ================================= ENUMS =================================== */

// Token types for lexer
typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_SEMICOLON,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_REDIR_APPEND,
	TOKEN_HEREDOC,
}	t_token_type;

// AST node types
typedef enum e_node_type
{
	NODE_COMMAND,
	NODE_PIPELINE,
	NODE_SEQUENCE,
}	t_node_type;

// Redirection types
typedef enum e_redir_type
{
	REDIR_IN,
	REDIR_OUT,
	REDIR_APPEND,
	REDIR_HEREDOC
}	t_redir_type;

/* ================================= STRUCTS ================================= */

char	*handle_input(char *prompt);
void	redirect_tty(int *tty, int *saved_in, int *saved_out);
void	restore_stdio(int saved_in, int saved_out);

// Forward declarations
typedef struct s_ast_node		t_ast_node;
typedef struct s_shell			t_shell;

// Token structure for lexer
typedef struct s_token
{
	t_token_type			type;
	char					*value;
	struct s_token			*next;
}	t_token;

// Lexer structure
typedef struct s_lexer
{
	char					*input;
	int						position;
	int						length;
	char					current_char;
}	t_lexer;

// Redirection structure
typedef struct s_redirect
{
	t_redir_type			type;
	char					*filename;
	int						quoted_delimiter;
	struct s_redirect		*next;
}	t_redirect;

// Command structure
typedef struct s_command
{
	char					**args;
	int						argc;
	t_redirect				*redirects;
}	t_command;

// AST node structure
typedef struct s_ast_node
{
	t_node_type				type;
	union
	{
		t_command			*command;
		struct
		{
			t_ast_node		*left;
			t_ast_node		*right;
		}	s_binary;        // for pipes
	}	u_data;
}	t_ast_node;

// Parser structure
typedef struct s_parser
{
	t_token					*tokens;
	t_token					*current;
}	t_parser;

// Environment item structure
typedef struct s_env_item
{
	char					*key;
	char					*value;
	struct s_env_item		*next;
}	t_env_item;

// Environment structure
typedef struct s_env
{
	t_env_item				*first_node;
	int						exit_status;
}	t_env;

// Garbage collector node
typedef struct s_trash
{
	void					*mem;
	struct s_trash			*next;
}	t_trash;

// Garbage collector structure
typedef struct s_gc
{
	t_trash					*first_node;
	t_trash					*gc_mark;
}	t_gc;

// GC node for advanced GC
typedef struct s_gc_node
{
	void					*ptr;
	size_t					size;
	bool					marked;
	struct s_gc_node		*next;
}	t_gc_node;

// Advanced GC structure
typedef struct s_gc_advanced
{
	t_gc_node				*head;
	size_t					total_allocated;
	size_t					allocation_count;
}	t_gc_advanced;

// Shell structure
typedef struct s_shell
{
	char					**env;
	int						env_len;
	int						last_status;
}	t_shell;

// Expander structure
typedef struct s_expander
{
	char					*prompt;
	char					**queue;
    int                     marker;
    int                     queue_marker;
}	t_expander;


/* ========================== FUNCTION DECLARATIONS ========================= */

/* --------------------------------- MAIN ----------------------------------- */
int			main(void);
void		*init_env(void);

/* --------------------------------- LEXER ---------------------------------- */
t_token		*lexer(char *input);
void		advance_char(t_lexer *lexer);
void		cleanup_tokens(t_token *tokens);
char		peek_char(t_lexer *lexer, int offset);
t_token		*create_token(t_token_type type, char *value);
void		add_token_to_list(t_token **head, t_token *new_token);

// Lexer checks
bool		is_quote(char c);
bool		is_operator(char c);
bool		is_word_char(char c);
bool		is_whitespace(char c);
bool		is_double_operator(t_lexer *lexer);

// Token reading
t_token		*read_word(t_lexer *lexer);
t_token		*read_operator(t_lexer *lexer);

/* --------------------------------- PARSER --------------------------------- */
t_ast_node	*parser(t_token *tokens);
t_ast_node	*create_ast_node(t_node_type type);
void		advance_token(t_parser *parser);
void		cleanup_redirections(t_redirect *redirects);
void		cleanup_command(t_command *cmd);
void		cleanup_ast(t_ast_node *ast);
bool		is_redirect_token(t_token_type type);
t_ast_node	*parse_ast_node(t_parser *parser);
t_redirect	*parse_redirections(t_parser *parser);
t_ast_node	*parse_primary(t_parser *parser);
t_ast_node	*parse_pipeline(t_parser *parser);
t_ast_node	*parse_sequence(t_parser *parser);

/* --------------------------------- EXECUTOR -------------------------------- */
int			exec_ast(const t_ast_node *ast);

/* --------------------------------- BUILTINS -------------------------------- */
int			is_builtin(const char *cmd);
int			run_builtin(char **argv);
char 		*get_cwd();
int 		bi_echo(char **argv);
int 		bi_pwd(void);
int 		bi_env(char **argv);
int 		bi_exit(char **argv);
int 		bi_cd(char **argv);
int 		bi_export(char **argv);
int 		bi_unset(char **argv);
char		**get_sorted_env(void);

/* --------------------------------- ENVIRONMENT ----------------------------- */
t_env_item	*new_env_item(char *key, char *val);
t_env_item	*is_env_item_exists(char *key);
t_env_item	*get_env_item(char *key);
t_env_item	*add_env_item(t_env_item *item);
void		unset_env_item(char *key);
void		print_env(void);
char		**get_env_array(void);
t_env		*get_env(void);
t_env		*new_env(void);

/* -------------------------------- SHELL CORE ------------------------------- */
int			shell_run(void);
char		*read_heredoc_input(const char *delimiter);

/* -------------------------------- HEREDOC UTILS ---------------------------- */
void		copy_content(char *dest, char *src, int len);
char		*resize_content(char *content, int *cap, int len, int line_len);
void		add_line_to_content(char **content, int *content_len,
				char *expanded, int line_len);
int			check_delimiter(char *line, const char *delimiter, int delim_len);
char		*get_heredoc_line(void);
char		*process_heredoc_line_raw(char *content, int *content_len,
				int *content_cap, char *line);
char		*expand_heredoc_content(char *content, int quoted);
char		*safe_heredoc(char *prompt);
void		print_heredoc_warning(char *delimiter);

/* -------------------------------- EXPANSION --------------------------------- */
//char		*expand_string(const char *in);
char		**expand_argv(char **argv, int argc);
char		*find_variable_in_string(char *str, char *var_name);
void		replace_single_variable(char *str, char *var_name, char *value);
void		apply_all_replacements(char *result);
char		*replace_all_variables(void);
char		*get_variable_value(char *key);
char		*expand(char *prompt);
int			calculate_result_size(void);
int			calculate_final_length(void);
int			valid_dollar_count(char *str);
t_expander	*get_expander(bool value);
char        *remove_outer_quotes(char *str);
int         has_quotes(const char *str);
void        queue_expandables();
int quote_status(char *str, int index);
bool is_char(char c);
bool is_number(char c);

/* -------------------------------- ENVIRONMENT ------------------------------- */
int			sh_env_set(const char *key, const char *val, int overwrite);
int			sh_env_unset(const char *key);
const char	*sh_getenv_val(const char *name);

/* -------------------------------- SIGNALS ----------------------------------- */
void		init_signals(void);
void		sh_signal_set_state(int state_type, int value);
int			sh_signal_interrupted(void);
int			sh_signal_should_exit(void);
void		sh_signal_reset(void);

/* -------------------------------- UTILS ------------------------------------- */
int			sh_is_line_empty(const char *s);
int			sh_strlen(const char *s);
char		*sh_strdup(const char *s);
int			sh_strcmp(const char *a, const char *b);
int			sh_strncmp(const char *a, const char *b, int n);
char		*sh_strchr(const char *s, int c);
char		*sh_join_path(const char *dir, const char *file);
char		**sh_split_colon(const char *s);
void		sh_free_strarray(char **arr);

/* -------------------------------- LIBFT ------------------------------------- */
size_t		ft_strlen(const char *str);
bool		is_str_empty(char *str);
bool		in(char *str, char c);
ssize_t		len(const char *str);
void		ft_strcpy(char *src, char *dst);
void		ft_strncpy(char *src, char *dst, int n);
bool		ft_strcmp(char *str, char *to_cmp);
long		ft_atol(char *str);
char		*ft_ltoa(long l);
char		**ft_split(char const *s, char c);

/* -------------------------------- GARBAGE COLLECTOR ------------------------ */
void		*alloc(ssize_t size);
t_gc		*get_gc(void);
t_gc		*new_gc(void);
t_trash		*new_trash(void *mem);
void		insert_to_gc(t_trash *new_trash);
void		dump_gc(void);

#endif
