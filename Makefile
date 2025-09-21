# ================================== COLORS ================================== #
RED     := \033[0;31m
GREEN   := \033[0;32m
YELLOW  := \033[0;33m
CYAN    := \033[0;36m
RESET   := \033[0m

# ================================ SETTINGS ================================== #
NAME        := minishell
CC          := cc
CFLAGS      := -Wall -Wextra -Werror -g
LDFLAGS     := -lreadline

# =============================== DIRECTORIES =============================== #
SRCDIR      := .
OBJDIR      := .obj
INCDIR      := .

# ================================ SOURCE FILES ============================== #
# Lexer sources
SRC_LEX     := \
	lexer/lexer.c \
	lexer/read_token.c \
	lexer/is_it.c \
	lexer/utils.c \
	lexer/libft.c

# Parser sources
SRC_PARSE   := \
	parser/parser.c \
	parser/parser_nodes.c \
	parser/ast.c \
	parser/redir.c \
	parser/redir_utils.c \
	parser/utils.c

# Garbage collector sources
SRC_GC      := \
	gc/gc.c \
	gc/cons.c \
	gc/dump.c

# Core shell sources
SRC_CORE    := \
	core/main.c \
	core/repl.c \
	core/repl_utils.c \
	core/input.c \
	core/heredoc.c \
	core/heredoc_utils.c \
	core/heredoc_input.c \
	core/heredoc_expand.c 

# Utility sources
SRC_UTILS   := \
	utils/utils_1.c \
	utils/utils_2.c \
	utils/utils_3.c \
	signals/signals.c

# Environment sources
SRC_ENV     := \
	environment/env_init.c \
	environment/env_constructors.c \
	environment/env_items.c \
	environment/env_print.c \
	environment/env_array.c

# Executor sources
SRC_EXEC    := \
	executor/executor.c \
	executor/executor_redirs.c \
	executor/executor_path.c \
	executor/executor_expand.c \
	executor/executor_exec.c \
	executor/executor_pipeline.c \
	executor/executor_sequence.c \
	executor/executor_command.c \
	executor/executor_heredoc_utils.c

# Builtin sources
SRC_BUILTIN := \
	builtin/builtins.c \
	builtin/cwd.c \
	builtin/ms_bi_env.c \
	builtin/sort_env.c \
	builtin/sort_env_utils.c \
	builtin/ms_cd.c \
	builtin/ms_echo.c \
	builtin/ms_env.c \
	builtin/ms_exit.c \
	builtin/ms_pwd.c

# Expander sources
SRC_EXPAND  := \
	expand/expand.c \
	expand/expand_main.c \
	expand/fetch.c \
	expand/expander_utils.c \
	expand/get_expander.c \
	expand/queue.c \
	expand/quotes.c \
	expand/new_prompt.c \
	expand/queue_quotes.c

# Library sources
SRC_LIBFT   := \
	libft/ft_split.c \
	libft/libft_0.c \
	libft/libft_string.c \
	libft/libft_number.c \
	libft/libft_convert.c \
	libft/ft_bzero.c

# Combine all sources
SRCS        := $(SRC_LEX) $(SRC_PARSE) $(SRC_GC) $(SRC_CORE) $(SRC_UTILS) \
               $(SRC_ENV) $(SRC_EXEC) $(SRC_BUILTIN) $(SRC_EXPAND) \
               $(SRC_LIBFT)

OBJS        := $(SRCS:%.c=$(OBJDIR)/%.o)

# ================================== RULES =================================== #

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJS)
	@printf "$(CYAN)🔗 Linking $(NAME)...$(RESET)\n"
	@$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)
	@printf "$(GREEN)✅ Build complete: $(NAME)$(RESET)\n"

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@printf "$(YELLOW)🔨 Compiling $<$(RESET)\n"
	@$(CC) $(CFLAGS) -c $< -o $@

# ============================== CLEANING ================================== #
clean:
	@printf "$(RED)🧹 Cleaning object files...$(RESET)\n"
	@rm -rf $(OBJDIR)

fclean: clean
	@printf "$(RED)🗑️  Removing $(NAME)...$(RESET)\n"
	@rm -f $(NAME)

re: fclean all