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

static void	remove_empty_arg(const t_command *cmd, int i)
{
	int	j;

	j = i;
	while (j < cmd->argc - 1)
	{
		cmd->args[j] = cmd->args[j + 1];
		j++;
	}
	((t_command *)cmd)->argc--;
	cmd->args[cmd->argc] = NULL;
}

static void	process_expanded_arg(const t_command *cmd, int *i, char *expanded)
{
	if (!expanded)
		return ;
	(cmd)->args[*i] = expanded;
	if (expanded[0] == '\0')
	{
		remove_empty_arg(cmd, *i);
		(*i)--;
	}
}

void	expand_args(const t_command *cmd)
{
	char	*expanded;
	int		i;

	if (!cmd || !cmd->args)
		return ;
	i = 0;
	while (i < cmd->argc && cmd->args[i])
	{
		expanded = expand(cmd->args[i]);
		process_expanded_arg(cmd, &i, expanded);
		i++;
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
