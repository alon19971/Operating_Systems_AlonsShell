#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

               // ----------EPISODE-C FUNCTIONS----------


//  Deletes a file specified by the path in the command arguments.
// Handles paths with spaces by expecting the path to be enclosed in quotes.
// If the path is not found or the file cannot be deleted, an error message is printed.
void delete(char **path) {
    // Check if there's a path provided
    if (path[1] == NULL) {
        printf("-myShell: delete: Missing file name\n");
        return;
    }

    char fullPath[1024] = {0}; // Buffer to hold the full path

    // Handle paths enclosed in quotes or containing spaces
    if (path[1][0] == '\"' || path[1][0] == '\'') { // Check if path starts with a quote
        // Assume that quotes are properly closed in the input
        int i = 1;
        while (path[i] != NULL) {
            // Remove leading quote for the first argument and trailing quote for the last
            char* start = (i == 1) ? path[i] + 1 : path[i];
            char* end = strstr(start, "\"") ? strstr(start, "\"") : strstr(start, "'");
            
            if (end) *end = '\0'; // Remove trailing quote if present
            strcat(fullPath, start);

            if (end || path[i + 1] == NULL) break; // Stop if end quote found or no more arguments
            
            strcat(fullPath, " "); // Add space between parts for mid arguments
            i++;
        }
    } else if (path[2] != NULL) { // Handle multiple arguments as parts of a path with spaces, not enclosed in quotes
        for (int i = 1; path[i] != NULL; i++) {
            strcat(fullPath, path[i]);
            if (path[i+1] != NULL) strcat(fullPath, " "); // Add space between parts
        }
    } else {
        // Single argument - simple path without spaces
        strcpy(fullPath, path[1]);
    }

    // Attempt to delete the file
    if (unlink(fullPath) != 0) {
        perror("Error deleting file");
        printf("-myShell: delete: %s: No such file or directory\n", fullPath);
    } else {
        printf("File '%s' deleted successfully\n", fullPath);
    }
}


// Creates a pipe between two commands, executing the first command's output as the input for the second command.
// It sets up a pipe, forks twice to create two processes for each command, redirects the output and input accordingly,
// and then executes each command in its respective process.
void mypipe(char **argv1, char **argv2) {
    int fields[2];
    pipe(fields); // This needs to be outside before fork() to setup pipe first

    if (fork() == 0) {
        // Child process for the first command
        close(STDOUT_FILENO);
        dup(fields[1]);
        close(fields[1]);
        close(fields[0]);
        execvp(argv1[0], argv1);
        exit(EXIT_FAILURE); // If execvp returns, it must have failed
    } else {
        // Parent process, fork again for the second command
        if (fork() == 0) {
            close(STDIN_FILENO);
            dup(fields[0]);
            close(fields[0]);
            close(fields[1]);
            execvp(argv2[0], argv2);
            exit(EXIT_FAILURE); // If execvp returns, it must have failed
        }
    }
    close(fields[0]);
    close(fields[1]);
    wait(NULL); // Wait for the first child
    wait(NULL); // Wait for the second child
}


// Prepares and executes a piped command by splitting the input command into two parts at the first occurrence of '|'.
// Each part is then tokenized into arguments and passed to `mypipe` for execution.
// If a pipe ('|') symbol is not found in the command, an error message is printed.
// This function is designed to parse and handle simple piped commands entered by the user.
void prepareAndExecutePipe(char *input) {
    char *parts[2] = {NULL, NULL}; // To store the split parts of the command
    char *argv1[256], *argv2[256]; // Assuming max 256 arguments for simplicity
    int idx1 = 0, idx2 = 0;

    // Split the input into two parts based on the first occurrence of '|'
    parts[0] = strtok(input, "|");
    if (parts[0] != NULL) {
        parts[1] = strtok(NULL, "");
    }

    if (parts[1] == NULL) {
        printf("Pipe ('|') not found in command.\n");
        return;
    }

    // Tokenize and fill argv1 and argv2 for execvp
    char *token = strtok(parts[0], " ");
    while (token) {
        argv1[idx1++] = token;
        token = strtok(NULL, " ");
    }
    argv1[idx1] = NULL; // Null-terminate argv1

    token = strtok(parts[1], " ");
    while (token) {
        argv2[idx2++] = token;
        token = strtok(NULL, " ");
    }
    argv2[idx2] = NULL; // Null-terminate argv2

    mypipe(argv1, argv2); // Call mypipe with the prepared argument arrays
}
