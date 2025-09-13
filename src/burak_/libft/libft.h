/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 01:13:06 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/13 18:07:40 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBMS_H
# define LIBMS_H
# include <stdbool.h>
# include <sys/types.h>

bool is_str_empty(char *str);
bool	in(char *str, char c);
ssize_t	len(char *str);
void ft_strcpy(char *src, char *dst);
void ft_strncpy(char *src, char *dst, int n);
bool ft_strcmp(char *str, char *to_cmp);
char	**ft_split(char const *s, char c);
void increase_istr(char *str);
long ft_atol(char *str);
char *ft_ltoa(long l);
char *ft_strdup(char *str);

#endif