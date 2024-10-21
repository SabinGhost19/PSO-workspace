#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define BUFFER_SIZE 100
char *extract_first_cmd(char *buffer)
{
    char *cmd = (char *)malloc(sizeof(char) * 100);
    int i = 0;
    while (buffer[i] != '\n' && buffer[i] != ' ')
    {
        cmd[i] = buffer[i];
        i++;
    }
    cmd[i] = '\0';

    return cmd;
}
char *read_line()
{

    char *buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE);
    printf("> ");
    ssize_t read_bytes = read(STDIN_FILENO, buffer, BUFFER_SIZE);

    if (read_bytes < 0)
    {
        perror("error reading the input");
        exit(-1);
    }

    buffer[read_bytes] = '\0';

    return buffer;
}
void simple_cmd(char *command)
{
    command[strcspn(command, "\n")] = '\0';
    char *args[10]; // Maxim 10 argumente
    int i = 0;
    // spargem comadna in argumente
    args[i] = strtok(command, " ");
    while (args[i] != NULL && i < 9)
    {
        i++;
        args[i] = strtok(NULL, " ");
    }

    pid_t pid = fork();
    switch (pid)
    {
    case 0:
        // child
        int status = execvp(args[0], args);
        break;
    case -1:
        perror("Eroare la fork");
        break;
    default:
        // parent
        wait(NULL);
        break;
    }
}
int main()
{

    setvbuf(stdout, NULL, _IONBF, 0);
    char first_cmd[100];
    while (strcmp(first_cmd, "exit") != 0)
    {
        char *line = read_line();
        strcpy(first_cmd, extract_first_cmd(line));
        simple_cmd(first_cmd);
    }
    return 0;
}