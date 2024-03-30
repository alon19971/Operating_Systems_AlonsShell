#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <fcntl.h>

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


// Lists the contents of a directory   -
// This function takes an array of arguments where the first argument after the command
// can optionally be the path to a directory. If no path is provided, it defaults to the
// current directory ('.'). It attempts to open the specified directory and iterates through
// its entries, printing each entry's name to stdout. If the directory cannot be opened,
// an error message is displayed.
void listDirectoryContents(char **args) {
    const char *dirPath = args[1] ? args[1] : ".";
    DIR *dir = opendir(dirPath);
    if (dir == NULL) {
        perror("ls");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
}


// Displays the contents of a file to stdout    -
// This function takes an array of arguments where the first argument after the command
// should be the path to a file. It opens the specified file in read mode and reads its
// contents line by line, printing each line to stdout. If the file cannot be opened,
// an error message is displayed. This function is akin to the Unix 'cat' command and
// is useful for viewing file contents directly from the shell.
void displayFileContents(char **args) {
    if (args[1] == NULL) {
        printf("cat: Missing file operand\n");
        return;
    }

    char *fileName = args[1]; // Assume the file name is the second argument

    // Remove quotes from the file name if present
    if (fileName[0] == '"' || fileName[0] == '\'') {
        fileName++; // Move the pointer past the opening quote
        // Find the closing quote and replace it with '\0'
        char *closingQuote = strchr(fileName, fileName[0]);
        if (closingQuote != NULL) {
            *closingQuote = '\0';
        }
    }

    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        perror("cat");
        return;
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), file)) {
        printf("%s", buffer);
    }

    fclose(file);
}


// Executes the echo command in a custom shell environment.
// This function mimics the behavior of the Unix 'echo' command, allowing for output redirection.
// It prints the provided arguments to stdout or redirects the output to a file if the '>' operator is detected.
void echo(char **args) {
    int fd = -1; // Initialize fd to -1 to indicate no file is opened initially
    if (args[1] == NULL) {
        printf("\n");
        return;
    }

    // Check for redirection '>'
    int i = 1;
    while (args[i] != NULL) {
        if (strcmp(args[i], ">") == 0 && args[i + 1] != NULL) {
            // Redirect output to a file
            fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1) {
                perror("open");
                return;
            }
            // Skip the '>' and file name in output
            args[i] = NULL;
            i -= 1;
            break;
        }
        i++;
    }

    for (int j = 1; args[j] != NULL; j++) {
        printf("%s", args[j]);
        if (args[j + 1] != NULL) printf(" ");
    }
    printf("\n");

    // If output was redirected (fd was changed), close the file descriptor
    if (fd != -1) {
        close(fd);
    }
}

