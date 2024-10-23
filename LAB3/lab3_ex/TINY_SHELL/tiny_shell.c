#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define BUFFER_SIZE 100

// Functie pentru extragerea primei comenzi
char *extract_first_cmd(char *buffer)
{
    char *cmd = (char *)malloc(sizeof(char) * 100);
    int i = 0;
    while (buffer[i] != '\n' && buffer[i] != ' ' && buffer[i] != '\0')
    {
        cmd[i] = buffer[i];
        i++;
    }
    cmd[i] = '\0';
    return cmd;
}

// Functie pentru citirea inputului din linia de comanda
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

    // Eliminam newline-ul '\n' de la final
    if (read_bytes > 0 && buffer[read_bytes - 1] == '\n')
    {
        buffer[read_bytes - 1] = '\0';
    }
    else
    {
        buffer[read_bytes] = '\0';
    }

    return buffer;
}
char *parse_command(char *line, char **args)
{

    char *arg_cmd = strtok(line, " ");
    // in lista de argumente primul argument
    // trebuie sa fie numele comenzii
    int i = 0;
    while (arg_cmd != NULL && i < 9)
    {
        args[i] = arg_cmd;
        i++;
        arg_cmd = strtok(NULL, " ");
    }
    args[i] = NULL; // final lists of arguments
    return args[0];
}

void simple_cmd(char *command, char *args[10])
{

    pid_t pid = fork();
    switch (pid)
    {
    case 0:
        // child
        if (execvp(command, args) == -1)
        {
            perror("execvp failed");
        }
        exit(EXIT_FAILURE);
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

    char *first_command;
    char *args[10]; // Argumentele comenzii
    // maxim 10 argumente

    while (1)
    {

        // citire linie
        char *line = read_line();

        // extragere first comand
        first_command = parse_command(line, args);

        if (strcmp(first_command, "exit") == 0)
        {
            free(line);
            break;
        }

        // exec
        simple_cmd(first_command, args);

        free(line);
    }

    return 0;
}
