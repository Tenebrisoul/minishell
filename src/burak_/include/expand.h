/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 22:40:09 by root              #+#    #+#             */
/*   Updated: 2025/09/13 15:12:19 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXPAND_H
# define EXPAND_H
# include <stdbool.h>
# include <sys/types.h>

# define INIT true
# define GET false

typedef struct s_expander t_expander;

struct s_expander
{
    char *prompt;
    int marker;
    char **queue;
    int queue_marker;
};

bool is_number(char c);
bool is_char(char c);

void *alloc(ssize_t size);

t_expander *get_expander(bool create);

int analyze_quotes(bool qt, bool dqt, char outer);
int quote_status(char *str, int index);
int valid_dollar_count(char *str);
void queue_expandables();
char *get_variable_value(char *var_name);

#endif