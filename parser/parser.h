/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ivar <ivar@student.42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 23:49:14 by ivar              #+#    #+#             */
/*   Updated: 2025/07/08 23:49:15 by ivar             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include "../lexer/lexer.h"

typedef enum e_node_type
{
	NODE_COMMAND,
	NODE_PIPELINE,
	NODE_SEQUENCE,
}	t_node_type;

typedef enum e_redir_type
{
	REDIR_IN,
	REDIR_OUT,
	REDIR_APPEND,
	REDIR_HEREDOC
}	t_redir_type;

typedef struct s_redirect
{
	t_redir_type		type;
	char				*filename;
	struct s_redirect	*next;
}	t_redirect;

typedef struct s_command
{
	char		**args;
	int			argc;
	t_redirect	*redirects;
}	t_command;

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
		}	s_pipeline;
		struct
		{
			struct s_ast_node	*left;
			struct s_ast_node	*right;
		}	s_sequence;
	}	u_data;
}	t_ast_node;

typedef struct s_parser
{
	t_token	*tokens;
	t_token	*current;
}	t_parser;

// parser.c
t_ast_node	*parser(t_token *tokens);

// ast.c
t_ast_node	*parse_ast_node(t_parser *parser);

// redir.c
bool		is_redirect_token(t_token_type type);
t_redirect	*parse_redirections(t_parser *parser);

// utils.c
void		cleanup_ast(t_ast_node *node);
void		advance_token(t_parser *parser);
void		cleanup_command(t_command *cmd);
t_ast_node	*create_ast_node(t_node_type type);
void		cleanup_redirections(t_redirect *redirects);

#endif