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
	token->type = type;
	token->value = sh_strdup(value);
	if (!token->value)
		return (NULL);
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
