/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:12:53 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/22 01:53:58 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static t_lexer	*init_lexer(char *input)
{
	t_lexer	*lexer;

	if (!input || !*input)
		return (NULL);
	lexer = alloc(sizeof(t_lexer));
	lexer->input = sh_strdup(input);
	if (!lexer->input)
		return (NULL);
	lexer->length = ft_strlen(input);
	lexer->position = 0;
	lexer->current_char = lexer->input[0];
	return (lexer);
}

static bool	tokenize(t_lexer *lexer, t_token **token_list)
{
	t_token	*current_token;

	while (lexer->current_char != '\0')
	{
		while (is_whitespace(lexer->current_char))
			advance_char(lexer);
		if (lexer->current_char == '\0')
			break ;
		if (is_operator(lexer->current_char))
			current_token = read_operator(lexer);
		else if (is_word_char(lexer->current_char))
			current_token = read_word(lexer);
		else
			current_token = NULL;
		if (!current_token)
			return (false);
		add_token_to_list(token_list, current_token);
	}
	return (true);
}

t_token	*lexer(char *input)
{
	t_lexer	*lexer;
	t_token	*token_list;

	if (!input || !*input)
		return (NULL);
	lexer = init_lexer(input);
	if (!lexer)
		return (NULL);
	token_list = NULL;
	if (!tokenize(lexer, &token_list))
		return (NULL);
	return (token_list);
}
