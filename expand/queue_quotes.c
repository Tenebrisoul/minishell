/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue_quotes.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 22:21:53 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/21 22:23:22 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	analyze_queue_quotes(bool qt, bool dqt)
{
	if (qt || dqt)
		return 1;
	return 0;
}

int	queue_quote_status(char *str, int index)
{
	bool	in_quote;
	bool	in_dquote;
	int		counter;

	in_quote = false;
	in_dquote = false;
	counter = -1;
	while (str[++counter] && counter < index)
	{
		if (str[counter] == '\'' && !in_dquote)
			in_quote = !in_quote;
		else if (str[counter] == '\"' && !in_quote)
			in_dquote = !in_dquote;
	}
	return (analyze_queue_quotes(in_quote, in_dquote));
}
