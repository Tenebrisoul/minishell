/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:25:09 by root              #+#    #+#             */
/*   Updated: 2025/09/19 12:19:32 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	advance_char(t_lexer *lexer)
{
	if (lexer->position < lexer->length)
	{
		lexer->position++;
		if (lexer->position < lexer->length)
			lexer->current_char = lexer->input[lexer->position];
		else
			lexer->current_char = '\0';
	}
	else
	{
		lexer->current_char = '\0';
	}
}

char	peek_char(t_lexer *lexer, int offset)
{
	int	pos;

	pos = lexer->position + offset;
	if (pos >= lexer->length || pos < 0)
		return ('\0');
	return (lexer->input[pos]);
}

t_token	*create_token(t_token_type type, char *value)
{
	t_token	*token;

	token = alloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	token->value = ft_strdup(value);
	if (!token->value)
	{
		free(token);
		return (NULL);
	}
	token->next = NULL;
	return (token);
}

void	add_token_to_list(t_token **head, t_token *new_token)
{
	t_token	*current;

	if (!*head)
	{
		*head = new_token;
		return ;
	}
	current = *head;
	while (current->next)
		current = current->next;
	current->next = new_token;
}

void	cleanup_tokens(t_token *tokens)
{
	t_token	*next;

	while (tokens)
	{
		next = tokens->next;
		if (tokens->value)
			free(tokens->value);
		free(tokens);
		tokens = next;
	}
}
