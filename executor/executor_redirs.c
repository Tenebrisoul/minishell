#include "../minishell.h"

static int	apply_redir_in(const t_redirect *r)
{
	int	fd;

	fd = open(r->filename, O_RDONLY);
	if (fd < 0)
	{
		write(2, "minishell: ", 11);
		write(2, r->filename, sh_strlen(r->filename));
		write(2, ": No such file or directory\n", 28);
		return (1);
	}
	if (dup2(fd, 0) < 0)
	{
		write(2, "minishell: dup2: ", 17);
		perror("");
		close(fd);
		return (1);
	}
	close(fd);
	return (0);
}

static int	apply_redir_out(const t_redirect *r)
{
	int	fd;

	fd = open(r->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
	{
		write(2, "minishell: ", 11);
		write(2, r->filename, sh_strlen(r->filename));
		if (errno == EISDIR)
			write(2, ": Is a directory\n", 17);
		else
			write(2, ": Permission denied\n", 20);
		return (1);
	}
	if (dup2(fd, 1) < 0)
	{
		write(2, "minishell: dup2: ", 17);
		perror("");
		close(fd);
		return (1);
	}
	close(fd);
	return (0);
}

static int	apply_redir_append(const t_redirect *r)
{
	int	fd;

	fd = open(r->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd < 0)
	{
		write(2, "minishell: ", 11);
		write(2, r->filename, sh_strlen(r->filename));
		if (errno == EISDIR)
			write(2, ": Is a directory\n", 17);
		else
			write(2, ": Permission denied\n", 20);
		return (1);
	}
	if (dup2(fd, 1) < 0)
	{
		write(2, "minishell: dup2: ", 17);
		perror("");
		close(fd);
		return (1);
	}
	close(fd);
	return (0);
}

static int	apply_redir_heredoc(const t_redirect *r)
{
	int	pipefd[2];

	if (pipe(pipefd) < 0)
	{
		write(2, "minishell: pipe: ", 17);
		perror("");
		return (1);
	}
	if (write_heredoc_content(pipefd, r->filename))
		return (1);
	if (dup2(pipefd[0], 0) < 0)
	{
		write(2, "minishell: dup2: ", 17);
		perror("");
		close(pipefd[0]);
		return (1);
	}
	close(pipefd[0]);
	return (0);
}

int	apply_redirs_with_restore(const t_redirect *r, int *saved_stdin, int *saved_stdout)
{
	*saved_stdin = dup(0);
	*saved_stdout = dup(1);
	if (*saved_stdin < 0 || *saved_stdout < 0)
	{
		if (*saved_stdin >= 0)
			close(*saved_stdin);
		if (*saved_stdout >= 0)
			close(*saved_stdout);
		return (1);
	}
	while (r)
	{
		if (r->type == REDIR_IN && apply_redir_in(r))
			return (1);
		else if (r->type == REDIR_OUT && apply_redir_out(r))
			return (1);
		else if (r->type == REDIR_APPEND && apply_redir_append(r))
			return (1);
		else if (r->type == REDIR_HEREDOC && apply_redir_heredoc(r))
			return (1);
		r = r->next;
	}
	return (0);
}

void	restore_fds(int saved_stdin, int saved_stdout)
{
	if (saved_stdin >= 0)
	{
		dup2(saved_stdin, 0);
		close(saved_stdin);
	}
	if (saved_stdout >= 0)
	{
		dup2(saved_stdout, 1);
		close(saved_stdout);
	}
}

int	apply_redirs(const t_redirect *r)
{
	while (r)
	{
		if (r->type == REDIR_IN && apply_redir_in(r))
			return (1);
		else if (r->type == REDIR_OUT && apply_redir_out(r))
			return (1);
		else if (r->type == REDIR_APPEND && apply_redir_append(r))
			return (1);
		else if (r->type == REDIR_HEREDOC && apply_redir_heredoc(r))
			return (1);
		r = r->next;
	}
	return (0);
}
