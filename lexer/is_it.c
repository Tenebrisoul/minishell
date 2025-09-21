/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   is_it.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:12:51 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/21 23:12:51 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

bool	is_whitespace(char c)
{
	if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
		return (true);
	return (false);
}

bool	is_quote(char c)
{
	if (c == '\'' || c == '"')
		return (true);
	return (false);
}

bool	is_operator(char c)
{
	if (c == '|' || c == '<' || c == '>' || c == ';')
		return (true);
	return (false);
}

bool	is_double_operator(t_lexer *lexer)
{
	char	current;
	char	next;

	current = lexer->current_char;
	next = peek_char(lexer, 1);
	if ((current == '>' && next == '>') || (current == '<' && next == '<'))
		return (true);
	return (false);
}

bool	is_word_char(char c)
{
	if (!is_whitespace(c) && !is_operator(c) && c != '\0')
		return (true);
	return (false);
}
