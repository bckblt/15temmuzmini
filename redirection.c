#include "minishell.h"

int ft_heredoc(char **redirections)
{
    int fd[2];
    char *line = NULL;
    char *delimiter = redirections[1];
    
    if (pipe(fd) == -1)
    {
        ft_putstr_fd("minishell: pipe error\n", STDERR_FILENO);
        return (-1);
    }
    while (1)
    {
        line = readline("> ");
        if (!line)
        {
            close(fd[1]);
            close(fd[0]);
            return (-1);
        }
        if (ft_strcmp(line, delimiter) == 0)
        {
            free(line);
            break;
        }
        write(fd[1], line, strlen(line));
        write(fd[1], "\n", 1);
        free(line);
    }
    close(fd[1]);
    return fd[0];
}
void fd_closer(char *file)
{
    ft_putstr_fd("minishell: ", STDERR_FILENO);
    ft_putstr_fd(file, STDERR_FILENO);
    ft_putstr_fd(": open error\n", STDERR_FILENO);
}
void apply_redirections(char **redirections, t_fd *fds)
{
    int i;
    char *type;
    char *file;
    i = 0;
    while (redirections[i]) 
    {
        type = redirections[i];
        file = redirections[i + 1];

        if (strcmp(type, ">") == 0) {
            int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1)
            {
                fd_closer(file);
                continue;
            }
            fds->stdout = dup(STDOUT_FILENO);
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        else if (strcmp(type, ">>") == 0)
        {
            int fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd == -1)
            {
                fd_closer(file);
                continue;
            }
            fds->stdout = dup(STDOUT_FILENO);
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        else if (strcmp(type, "<") == 0)
        {
            int fd = open(file, O_RDONLY);
            if (fd == -1)
            {
                fd_closer(file);
                continue;
            }
            fds->stdin = dup(STDIN_FILENO);
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        else if (strcmp(type, "<<") == 0)
        {
            int heredoc_fd = ft_heredoc(redirections + i);
            if (heredoc_fd == -1) {
                ft_putstr_fd("minishell: heredoc error\n", STDERR_FILENO);
                continue;
            }
            fds->stdin = dup(STDIN_FILENO);
            dup2(heredoc_fd, STDIN_FILENO);
            close(heredoc_fd);
        }
        i += 2;
    }
}