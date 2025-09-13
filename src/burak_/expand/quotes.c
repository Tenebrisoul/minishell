/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quotes.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 22:39:11 by root              #+#    #+#             */
/*   Updated: 2025/09/13 22:24:10 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>

int analyze_quotes(bool qt, bool dqt, char outer)
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

int quote_status(char *str, int index)
{
    char outer_quote;
    bool in_quote;
    bool in_dquote;
    int counter;

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

int is_outer_quote(char *str, int index)
{
    char quote_char;
    int i;

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
