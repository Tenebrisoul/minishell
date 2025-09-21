/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_expand.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: btuncer <btuncer@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 23:11:57 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/21 23:11:58 by btuncer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	check_for_split(char *s)
{
	int	i;
	int	has_quote;
	int	has_dollar;

	i = 0;
	has_quote = 0;
	has_dollar = 0;
	while (s[i])
	{
		if (s[i] == '$')
			has_dollar = 1;
		if (s[i] == '\'' || s[i] == '\"')
			has_quote = 1;
		i++;
	}
	return (!has_quote && has_dollar);
}

static char	**build_new_args(t_command *cmd, char **split_result,
		int split_count, int i)
{
	char	**new_args;
	int		j;
	int		k;
	int		new_argc;

	new_argc = cmd->argc + split_count - 1;
	new_args = alloc(sizeof(char *) * (new_argc + 1));
	j = -1;
	while (++j < i)
		new_args[j] = cmd->args[j];
	k = -1;
	while (++k < split_count)
		new_args[j++] = sh_strdup(split_result[k]);
	k = i;
	while (++k < cmd->argc)
		new_args[j++] = cmd->args[k];
	new_args[j] = NULL;
	cmd->argc = new_argc;
	return (new_args);
}

static void	handle_split_arg(t_command *cmd, int *i)
{
	char	**split_result;
	int		j;

	split_result = ft_split(expand(cmd->args[*i]), ' ');
	j = 0;
	while (split_result && split_result[j])
		j++;
	cmd->args = build_new_args(cmd, split_result, j, *i);
	*i += j;
}

void	expand_args(const t_command *cmds)
{
	int			i;
	t_command	*cmd;

	cmd = (t_command *)cmds;
	if (!cmd || !cmd->args)
		return ;
	i = 0;
	while (i < cmd->argc && cmd->args[i])
	{
		if (check_for_split(cmd->args[i]))
			handle_split_arg(cmd, &i);
		else
		{
			cmd->args[i] = expand(cmd->args[i]);
			i++;
		}
	}
}

void	expand_redirects(const t_command *cmd)
{
	const t_redirect	*rr;
	char				*e;

	rr = cmd->redirects;
	while (rr)
	{
		if (rr->filename && rr->type != REDIR_HEREDOC)
		{
			e = expand(rr->filename);
			if (e)
				((t_redirect *)rr)->filename = e;
		}
		rr = rr->next;
	}
}
