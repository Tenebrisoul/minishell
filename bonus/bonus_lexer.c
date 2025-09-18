#include "../minishell.h"

bool is_logical_operator(t_lexer *lexer)
{
    if (lexer->current_char == '&' && peek_char(lexer, 1) == '&')
        return (true);
    if (lexer->current_char == '|' && peek_char(lexer, 1) == '|')
        return (true);
    return (false);
}

t_token *read_logical_operator(t_lexer *lexer)
{
    t_token *token;
    char *value;

    if (lexer->current_char == '&' && peek_char(lexer, 1) == '&')
    {
        value = alloc(3);
        if (!value)
            return (NULL);
        value[0] = '&';
        value[1] = '&';
        value[2] = '\0';
        advance_char(lexer);
        advance_char(lexer);
        token = create_token(TOKEN_AND, value);
    }
    else if (lexer->current_char == '|' && peek_char(lexer, 1) == '|')
    {
        value = alloc(3);
        if (!value)
            return (NULL);
        value[0] = '|';
        value[1] = '|';
        value[2] = '\0';
        advance_char(lexer);
        advance_char(lexer);
        token = create_token(TOKEN_OR, value);
    }
    else
    {
        return (NULL);
    }

    return (token);
}

t_token *read_parentheses(t_lexer *lexer)
{
    t_token *token;
    char *value;

    value = alloc(2);
    if (!value)
        return (NULL);

    if (lexer->current_char == '(')
    {
        value[0] = '(';
        value[1] = '\0';
        token = create_token(TOKEN_LPAREN, value);
    }
    else if (lexer->current_char == ')')
    {
        value[0] = ')';
        value[1] = '\0';
        token = create_token(TOKEN_RPAREN, value);
    }
    else
    {
        free(value);
        return (NULL);
    }

    advance_char(lexer);
    return (token);
}

t_token *read_wildcard(t_lexer *lexer)
{
    t_token *token;
    char *value;
    int start_pos;
    int len;
    int i;

    start_pos = lexer->position;

    // Read the entire word that might contain wildcards
    while (lexer->position < lexer->length &&
           (is_word_char(lexer->current_char) || lexer->current_char == '*'))
    {
        advance_char(lexer);
    }

    len = lexer->position - start_pos;
    value = alloc(len + 1);
    if (!value)
        return (NULL);

    i = 0;
    while (i < len)
    {
        value[i] = lexer->input[start_pos + i];
        i++;
    }
    value[len] = '\0';

    // Check if it contains wildcards
    if (sh_strchr(value, '*'))
        token = create_token(TOKEN_WILDCARD, value);
    else
        token = create_token(TOKEN_WORD, value);

    return (token);
}