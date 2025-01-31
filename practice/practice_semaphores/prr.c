#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

#define PIPE_READ 0
#define PIPE_WRITE 1
#define BUFSIZE 1024

// Funcția procesului copil care citește din fișier și trimite prin pipe
void child_process(int pipe_fd[2])
{
    close(pipe_fd[PIPE_READ]);           // Închidem capătul de citire al pipe-ului în copil
    FILE *file = fopen("data.txt", "r"); // Deschidem fișierul pentru citire
    if (file == NULL)
    {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    char line[BUFSIZE];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        printf("Line read: %s\n", line);
        // Trimite linia citită prin pipe
        write(pipe_fd[PIPE_WRITE], line, strlen(line) + 1);
    }

    fclose(file);               // Închidem fișierul
    close(pipe_fd[PIPE_WRITE]); // Închidem capătul de scriere al pipe-ului în copil
    exit(0);                    // Terminăm procesul copil
}

// Funcția procesului părinte care citește datele din pipe și le afișează
void parent_process(int pipe_fd[2])
{
    close(pipe_fd[PIPE_WRITE]); // Închidem capătul de scriere al pipe-ului în părinte
    char buffer[BUFSIZE];

    // Citim din pipe și afișăm linia citită
    int bytes_read;
    while ((bytes_read = read(pipe_fd[PIPE_READ], buffer, sizeof(buffer))) > 0)
    {
        printf("Parent received: %s", buffer); // Afișează linia primită
    }

    close(pipe_fd[PIPE_READ]); // Închidem capătul de citire al pipe-ului în părinte
}

int main()
{
    int pipe_fd[2];
    pid_t pid;

    // Creăm pipe-ul
    if (pipe(pipe_fd) == -1)
    {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }

    pid = fork(); // Creăm procesul copil
    if (pid == -1)
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        // Codul procesului copil
        child_process(pipe_fd);
    }
    else
    {
        // Codul procesului părinte
        parent_process(pipe_fd);
        wait(NULL); // Așteaptă terminarea procesului copil
    }

    return 0;
}
