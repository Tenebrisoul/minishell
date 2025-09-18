#include "../minishell.h"

t_ast_node *parse_logical_and(t_parser *parser)
{
    t_ast_node *left;
    t_ast_node *node;

    left = parse_pipeline(parser);
    if (!left)
        return (NULL);

    while (parser->current && parser->current->type == TOKEN_AND)
    {
        advance_token(parser);
        node = create_ast_node(NODE_AND);
        if (!node)
            return (NULL);
        node->u_data.s_binary.left = left;
        node->u_data.s_binary.right = parse_pipeline(parser);
        if (!node->u_data.s_binary.right)
            return (NULL);
        left = node;
    }

    return (left);
}

t_ast_node *parse_logical_or(t_parser *parser)
{
    t_ast_node *left;
    t_ast_node *node;

    left = parse_logical_and(parser);
    if (!left)
        return (NULL);

    while (parser->current && parser->current->type == TOKEN_OR)
    {
        advance_token(parser);
        node = create_ast_node(NODE_OR);
        if (!node)
            return (NULL);
        node->u_data.s_binary.left = left;
        node->u_data.s_binary.right = parse_logical_and(parser);
        if (!node->u_data.s_binary.right)
            return (NULL);
        left = node;
    }

    return (left);
}

t_ast_node *parse_expression(t_parser *parser)
{
    return (parse_sequence(parser));
}

int execute_logical_and(const t_ast_node *node, t_shell *shell)
{
    int left_status;

    (void)shell; // Unused parameter
    if (!node || !node->u_data.s_binary.left)
        return (1);

    // Execute left side
    left_status = exec_ast(node->u_data.s_binary.left);

    // Short-circuit: if left side fails, don't execute right side
    if (left_status != 0)
        return (left_status);

    // Execute right side
    if (node->u_data.s_binary.right)
        return (exec_ast(node->u_data.s_binary.right));

    return (0);
}

int execute_logical_or(const t_ast_node *node, t_shell *shell)
{
    int left_status;

    (void)shell; // Unused parameter
    if (!node || !node->u_data.s_binary.left)
        return (1);

    // Execute left side
    left_status = exec_ast(node->u_data.s_binary.left);

    // Short-circuit: if left side succeeds, don't execute right side
    if (left_status == 0)
        return (0);

    // Execute right side
    if (node->u_data.s_binary.right)
        return (exec_ast(node->u_data.s_binary.right));

    return (left_status);
}