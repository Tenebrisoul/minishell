#include "lexer.h"
#include <stdlib.h>

static t_lexer	*init_lexer(char *input)
{
	t_lexer	*lexer;

	if (!input || !*input)
		return (NULL);
	lexer = malloc(sizeof(t_lexer));
	if (!lexer)
		return (NULL);
	lexer->input = ft_strdup(input);
	if (!lexer->input)
	{
		free(lexer);
		return (NULL);
	}
	lexer->length = ft_strlen(input);
	lexer->position = 0;
	lexer->current_char = lexer->input[0];
	return (lexer);
}

static void	cleanup_lexer(t_lexer *lexer)
{
	if (lexer)
	{
		free(lexer->input);
		free(lexer);
	}
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
	{
		cleanup_tokens(token_list);
		cleanup_lexer(lexer);
		return (NULL);
	}
	cleanup_lexer(lexer);
	return (token_list);
}
