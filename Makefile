# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: root <root@student.42.fr>                  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/27 01:14:03 by btuncer           #+#    #+#              #
#    Updated: 2025/09/18 23:12:37 by root             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ================================== COLORS ================================== #
RED     := \033[0;31m
GREEN   := \033[0;32m
YELLOW  := \033[0;33m
BLUE    := \033[0;34m
MAGENTA := \033[0;35m
CYAN    := \033[0;36m
WHITE   := \033[0;37m
RESET   := \033[0m

# ================================ SETTINGS ================================== #
NAME        := minishell
CC          := cc
CFLAGS      := -Wall -Wextra -Werror -g
LDFLAGS     := -lreadline

# =============================== DIRECTORIES =============================== #
SRCDIR      := .
OBJDIR      := obj
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
	parser/ast.c \
	parser/redir.c \
	parser/utils.c

# Garbage collector sources
SRC_GC      := \
	gc/gc.c \
	gc/cons.c

# Core shell sources
SRC_CORE    := \
	core/main.c \
	core/repl.c \
	core/shell_run.c \
	core/input.c \
	core/heredoc.c \
	core/heredoc_utils.c \
	core/heredoc_input.c

# Utility sources
SRC_UTILS   := \
	utils/utils.c \
	signals/signals.c

# Environment sources
SRC_ENV     := \
	environment/env_init.c \
	environment/env_constructors.c \
	environment/env.c

# Executor sources
SRC_EXEC    := \
	executor/executor.c \
	executor/executor_redirs.c \
	executor/executor_path.c \
	executor/executor_expand.c \
	executor/executor_exec.c

# Builtin sources
SRC_BUILTIN := \
	builtin/builtins.c \
	builtin/cwd.c \
	builtin/ms_bi_env.c \
	builtin/sort_env.c \
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
	expand/quotes.c

# Library sources
SRC_LIBFT   := \
	libft/ft_split.c \
	libft/libft_0.c

# Combine all sources
SRCS        := $(SRC_LEX) $(SRC_PARSE) $(SRC_GC) $(SRC_CORE) $(SRC_UTILS) \
               $(SRC_ENV) $(SRC_EXEC) $(SRC_BUILTIN) $(SRC_EXPAND) \
               $(SRC_LIBFT)

OBJS        := $(SRCS:%.c=$(OBJDIR)/%.o)
DEPS        := $(OBJS:.o=.d)

# Include paths
INCLUDES    := -I$(INCDIR) -Ilexer -Iparser -Igc -Icore -Iutils -Isignals \
               -Ienvironment -Iexecutor -Ibuiltin -Iexpand -Ilibft

# ================================== RULES =================================== #

.PHONY: all clean fclean re info help norminette

# Default target
all: $(NAME)

# Main executable
$(NAME): $(OBJS)
	@printf "$(CYAN)🔗 Linking $(NAME)...$(RESET)\n"
	@$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(OBJS) $(LDFLAGS)
	@printf "$(GREEN)✅ Build complete: $(NAME)$(RESET)\n"

# Object files with dependency generation
$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@printf "$(YELLOW)🔨 Compiling $<$(RESET)\n"
	@$(CC) $(CFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

# Include dependency files
-include $(DEPS)

# ============================== CLEANING ================================== #
clean:
	@printf "$(RED)🧹 Cleaning object files...$(RESET)\n"
	@rm -rf $(OBJDIR)

fclean: clean
	@printf "$(RED)🗑️  Removing $(NAME)...$(RESET)\n"
	@rm -f $(NAME)

re: fclean all

# ============================== CODE QUALITY =============================== #
norminette:
	@printf "$(CYAN)📏 Running norminette...$(RESET)\n"
	@norminette $(SRCS) minishell.h

# ============================== UTILITIES ================================== #
info:
	@printf "$(CYAN)📊 Build Information:$(RESET)\n"
	@printf "  Name: $(GREEN)$(NAME)$(RESET)\n"
	@printf "  Compiler: $(GREEN)$(CC)$(RESET)\n"
	@printf "  Flags: $(GREEN)$(CFLAGS)$(RESET)\n"
	@printf "  Mode: $(GREEN)$(MODE)$(RESET)\n"
	@printf "  Sources: $(GREEN)$(words $(SRCS))$(RESET) files\n"
	@printf "  Objects: $(GREEN)$(OBJDIR)$(RESET)\n"
	@printf "  Includes: $(GREEN)$(INCLUDES)$(RESET)\n"

help:
	@printf "$(CYAN)🆘 Minishell Makefile Help$(RESET)\n"
	@printf "$(WHITE)Available targets:$(RESET)\n"
	@printf "  $(GREEN)all$(RESET)           - Build the project (default)\n"
	@printf "  $(GREEN)clean$(RESET)         - Remove object files\n"
	@printf "  $(GREEN)fclean$(RESET)        - Remove object files and executable\n"
	@printf "  $(GREEN)re$(RESET)            - Clean and rebuild\n"
	@printf "  $(GREEN)debug$(RESET)         - Build with debug flags and sanitizers\n"
	@printf "  $(GREEN)release$(RESET)       - Build optimized release version\n"
	@printf "  $(GREEN)norminette$(RESET)    - Check code style with norminette\n"
	@printf "  $(GREEN)check-leaks$(RESET)   - Run with Valgrind leak detection\n"
	@printf "  $(GREEN)info$(RESET)          - Show build information\n"
	@printf "  $(GREEN)help$(RESET)          - Show this help message\n"
	@printf "$(WHITE)Examples:$(RESET)\n"
	@printf "  make debug        # Build with debug flags\n"
	@printf "  make test-bonus   # Test all bonus features\n"
	@printf "  make check-leaks  # Check for memory leaks\n"