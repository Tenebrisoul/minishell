#include "../minishell.h"

static char	*expand_heredoc(char *prompt)
{
	t_expander	*expander;
	char		*expanded_prompt;
	char		*final_prompt;
	char		*processed_prompt;

	expander = get_expander(INIT);
	expander->prompt = prompt;
	expander->queue = alloc((valid_dollar_count(expander->prompt) + 1)
			* sizeof(char *));
	expander->queue[valid_dollar_count(expander->prompt)] = NULL;
	queue_expandables();
	expanded_prompt = replace_all_variables();
	if (expanded_prompt)
	{
		final_prompt = expanded_prompt;
		if (final_prompt)
		{
			processed_prompt = process_escapes(final_prompt);
			if (processed_prompt)
				return (processed_prompt);
			return (final_prompt);
		}
	}
	return (NULL);
}

char	*expand_heredoc_content(char *content, int quoted)
{
	char	*expanded_content;

	if (quoted)
		return (content);
	expanded_content = expand_heredoc(content);
	if (!expanded_content)
		return (content);
	return (expanded_content);
}
