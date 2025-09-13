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
	gc/gc.c

SRC_SHELL := \
	core/main.c \
	core/repl.c \
	utils/utils.c \
	signals/signals.c \
	env/env.c \
	executor/executor.c \
	builtins/builtins.c \
	expand/expand.c

SRCS := $(SRC_LEX) $(SRC_PARSE) $(SRC_GC) $(SRC_SHELL)
OBJS := $(SRCS:.c=.o)

INCLUDES := -Ilexer -Iparser -Igc -Icore -Iutils -Isignals -Ienv -Iexecutor -Ibuiltins -Iexpand
LDFLAGS := -lreadline

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

p: re
	rm -f $(OBJS)

.PHONY: all clean fclean re p
