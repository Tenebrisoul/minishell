/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ivar <ivar@student.42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 20:39:29 by ivar              #+#    #+#             */
/*   Updated: 2025/07/08 21:42:48 by ivar             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_H
# define LEXER_H

# include <stdbool.h>
# include <stddef.h>

typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_SEMICOLON,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_REDIR_APPEND,
	TOKEN_HEREDOC
}	t_token_type;

typedef struct s_token
{
	t_token_type	type;
	char			*value;
	struct s_token	*next;
}	t_token;

typedef struct s_lexer
{
	char			*input;
	int				position;
	int				length;
	char			current_char;
}	t_lexer;

t_token		*lexer(char *input);

void		advance_char(t_lexer *lexer);
void		cleanup_tokens(t_token *tokens);
char		peek_char(t_lexer *lexer, int offset);
t_token		*create_token(t_token_type type, char *value);
void		add_token_to_list(t_token **head, t_token *new_token);

bool		is_quote(char c);
bool		is_operator(char c);
bool		is_word_char(char c);
bool		is_whitespace(char c);
bool		is_double_operator(t_lexer *lexer);

t_token		*read_word(t_lexer *lexer);
t_token		*read_operator(t_lexer *lexer);

size_t		ft_strlen(const char *str);
char		*ft_strdup(const char *str);

#endif