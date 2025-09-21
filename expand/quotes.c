/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quotes.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:12:38 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/22 02:31:07 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	analyze_quotes(bool qt, bool dqt, char outer)
{
	if (qt && dqt)
	{
		if (outer == 'd')
			return (0);
		else if (outer == 'q')
			return (1);
	}
	else if (qt)
		return (0);
	else if (dqt)
		return (1);
	return (1);
}

int	quote_status(char *str, int index)
{
	char	outer_quote;
	bool	in_quote;
	bool	in_dquote;
	int		counter;

	outer_quote = 'N';
	in_quote = false;
	in_dquote = false;
	counter = -1;
	while (str[++counter] && counter < index)
	{
		if (str[counter] == '\'' && !in_dquote)
		{
			in_quote = !in_quote;
			if (in_quote)
				outer_quote = 's';
		}
		else if (str[counter] == '\"' && !in_quote)
		{
			in_dquote = !in_dquote;
			if (in_dquote)
				outer_quote = 'd';
		}
	}
	return (analyze_quotes(in_quote, in_dquote, outer_quote));
}

int	is_outer_quote(char *str, int index)
{
	char	quote_char;
	int		i;

	if (str[index] != '\'' && str[index] != '\"')
		return (0);
	quote_char = str[index];
	i = index + 1;
	while (str[i] && str[i] != quote_char)
		i++;
	if (str[i] == quote_char)
		return (i - index + 1);
	return (0);
}

char	*remove_outer_quotes(char *str)
{
	char	*result;
	char	quote_char;
	int		i;
	int		j;

	result = alloc((len(str) + 1) * sizeof(char));
	i = 0;
	j = 0;
	while (str[i])
	{
		if ((str[i] == '\'' || str[i] == '\"'))
		{
			quote_char = str[i];
			i++;
			while (str[i] && str[i] != quote_char)
				result[j++] = str[i++];
			if (str[i] == quote_char)
				i++;
		}
		else
			result[j++] = str[i++];
	}
	result[j] = '\0';
	return (result);
}
