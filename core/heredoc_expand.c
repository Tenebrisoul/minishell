/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_expand.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 21:00:00 by root              #+#    #+#             */
/*   Updated: 2025/09/19 21:00:00 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*expand_heredoc_content(char *content, int quoted)
{
	char	*expanded_content;

	if (quoted)
		return (content);
	expanded_content = expand(content);
	if (!expanded_content)
		return (content);
	return (expanded_content);
}
