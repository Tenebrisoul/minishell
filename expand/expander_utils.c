#include "minishell.h"

bool is_number(char c)
{
    return (c >= '0' && c <= '9');
}

bool is_char(char c)
{
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}