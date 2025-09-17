NAME := minishell
CC := cc
CFLAGS := -Wall -Wextra -Werror -g

SRC_LEX := \
	lexer/lexer.c \
	lexer/read_token.c \
	lexer/is_it.c \
	lexer/utils.c \
	lexer/libft.c

SRC_PARSE := \
	parser/parser.c \
	parser/ast.c \
	parser/redir.c \
	parser/utils.c

SRC_GC := \
	gc/gc.c \
	gc/cons.c

SRC_SHELL := \
	core/main.c \
	core/repl.c \
	core/shell_run.c \
	utils/utils.c \
	signals/signals.c \
	environment/env_init.c \
	environment/env_constructors.c \
	environment/env.c \
	executor/executor.c \
	builtin/builtins.c \
	builtin/cwd.c \
	expand/expand.c \
	expand/expand_main.c \
	expand/fetch.c \
	expand/expander_utils.c \
	expand/get_expander.c \
	expand/queue.c \
	expand/quotes.c \
	libft/ft_split.c \
	libft/libft_0.c

SRCS := $(SRC_LEX) $(SRC_PARSE) $(SRC_GC) $(SRC_SHELL)
OBJS := $(SRCS:.c=.o)

INCLUDES := -I.
LDFLAGS := -lreadline

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(OBJS) $(LDFLAGS)
	rm -f $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
