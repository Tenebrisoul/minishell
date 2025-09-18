#include "../minishell.h"

t_ast_node *parse_parentheses(t_parser *parser)
{
    t_ast_node *node;
    t_ast_node *child;

    if (!parser->current || parser->current->type != TOKEN_LPAREN)
        return (NULL);

    advance_token(parser); // consume '('

    // Parse the expression inside parentheses
    child = parse_expression(parser);
    if (!child)
        return (NULL);

    // Expect closing parenthesis
    if (!parser->current || parser->current->type != TOKEN_RPAREN)
    {
        // Syntax error: unclosed parentheses
        cleanup_ast(child);
        return (NULL);
    }

    advance_token(parser); // consume ')'

    // Create subshell node
    node = create_ast_node(NODE_SUBSHELL);
    if (!node)
    {
        cleanup_ast(child);
        return (NULL);
    }

    node->u_data.s_subshell.child = child;
    return (node);
}

t_ast_node *parse_primary(t_parser *parser)
{
    if (!parser->current)
        return (NULL);

    // Handle parentheses
    if (parser->current->type == TOKEN_LPAREN)
        return (parse_parentheses(parser));

    // Handle regular commands
    return (parse_ast_node(parser));
}

int execute_subshell(const t_ast_node *node, t_shell *shell)
{
    pid_t pid;
    int status;

    (void)shell; // Unused parameter
    if (!node || !node->u_data.s_subshell.child)
        return (1);

    // Fork a new process for the subshell
    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        return (1);
    }

    if (pid == 0)
    {
        // Child process: execute the subshell content
        int exit_status = exec_ast(node->u_data.s_subshell.child);
        exit(exit_status);
    }
    else
    {
        // Parent process: wait for child
        if (waitpid(pid, &status, 0) == -1)
        {
            perror("waitpid");
            return (1);
        }

        if (WIFEXITED(status))
            return (WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
            return (128 + WTERMSIG(status));
        else
            return (1);
    }
}