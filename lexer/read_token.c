#include "minishell.h"

t_token	*read_operator(t_lexer *lexer)
{
	char	c;

	c = lexer->current_char;
	if (is_double_operator(lexer))
	{
		advance_char(lexer);
		advance_char(lexer);
		if (c == '>')
			return (create_token(TOKEN_REDIR_APPEND, ">>"));
		else if (c == '<')
			return (create_token(TOKEN_HEREDOC, "<<"));
	}
	else if (is_operator(c))
	{
		advance_char(lexer);
		if (c == '|')
			return (create_token(TOKEN_PIPE, "|"));
		else if (c == ';')
			return (create_token(TOKEN_SEMICOLON, ";"));
		else if (c == '<')
			return (create_token(TOKEN_REDIR_IN, "<"));
		else if (c == '>')
			return (create_token(TOKEN_REDIR_OUT, ">"));
	}
	return (NULL);
}

static size_t	word_length(t_lexer *lexer)
{
	char	c;
	size_t	length;
	char	in_quote;

	length = 0;
	in_quote = 0;
	while (1)
	{
		c = peek_char(lexer, length);
		if (c == '\0')
			break ;
		if (is_quote(c))
		{
			if (in_quote == 0)
				in_quote = c;
			else if (in_quote == c)
				in_quote = 0;
		}
		else if (in_quote == 0 && !is_word_char(c))
			break ;
		length++;
	}
	if (in_quote != 0)
		return (0);
	return (length);
}

t_token	*read_word(t_lexer *lexer)
{
	t_token	*token;
	char	*content;
	size_t	length;
	size_t	i;

	i = 0;
	length = word_length(lexer);
	if (length == 0)
		return (NULL);
	content = malloc(length + 1);
	if (!content)
		return (NULL);
	content[length] = '\0';
	while (i < length)
	{
		content[i] = lexer->current_char;
		advance_char(lexer);
		i++;
	}
	token = create_token(TOKEN_WORD, content);
	free(content);
	return (token);
}
