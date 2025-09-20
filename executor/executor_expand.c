/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_expand.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:24:23 by root              #+#    #+#             */
/*   Updated: 2025/09/19 15:14:03 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	check_for_split(char *s)
{
	int	i;
	int has_quote;
	int has_dollar;

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

void	expand_args(const t_command *cmds)
{
	int		i;
	int		j;
	int		k;
	char	**split_result;
	char	**new_args;
	int		split_count;
	int		new_argc;
	t_command	*cmd;

	cmd = (t_command *)cmds;
	if (!cmd || !cmd->args)
		return ;
	i = 0;
	while (i < cmd->argc && cmd->args[i])
	{
		if (check_for_split(cmd->args[i]))
		{
			split_result = ft_split(expand(cmd->args[i]), ' ');
			split_count = 0;
			while (split_result && split_result[split_count])
				split_count++;
			new_argc = cmd->argc + split_count - 1;
			new_args = alloc(sizeof(char *) * (new_argc + 1));
			j = 0;
			while (j < i)
			{
				new_args[j] = cmd->args[j];
				j++;
			}
			k = 0;
			while (k < split_count)
			{
				new_args[j] = sh_strdup(split_result[k]);
				j++;
				k++;
			}
			k = i + 1;
			while (k < cmd->argc)
			{
				new_args[j] = cmd->args[k];
				j++;
				k++;
			}
			new_args[j] = NULL;
			cmd->args = new_args;
			cmd->argc = new_argc;
			i += split_count;
		}
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
