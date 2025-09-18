/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 18:52:31 by btuncer           #+#    #+#             */
/*   Updated: 2025/09/18 16:17:53 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILTINS_H
# define BUILTINS_H

char *get_cwd();
int bi_echo(char **argv);
int bi_pwd(void);
int bi_env(char **argv);
int bi_exit(char **argv);
int bi_cd(char **argv);
int bi_export(char **argv);
int bi_unset(char **argv);

#endif