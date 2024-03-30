#include "episodeD.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <libgen.h>

                     
                // --------EPISODE D FUNCTIONS--------


// Removes leading and trailing quotes from a string.
// This function modifies the string in place by adjusting the pointer
// to skip leading quotes and null-terminating the string early to remove trailing quotes.
// It handles both single ('') and double ("") quotes.
void removeQuotes(char *str) {
    if (!str) return; // Guard against null pointers

    size_t len = strlen(str);

    // Check and remove leading quote
    if (str[0] == '"' || str[0] == '\'') {
        memmove(str, str + 1, len); // Shift everything to the left by one
        len--; // Adjust length since we removed a character
    }

    // Check and remove trailing quote
    if (len > 0 && (str[len - 1] == '"' || str[len - 1] == '\'')) {
        str[len - 1] = '\0'; // Replace trailing quote with null terminator
    }
}


// This function handles the moving of a file from a specified source path
// to a destination path. It supports file paths containing spaces enclosed
// in quotes. If the destination path is a directory, the source file is moved
// into that directory, preserving its original name.
void move(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        printf("move: missing file operand\n");
        return;
    }

    removeQuotes(args[1]);
    removeQuotes(args[2]);

    printf("After removing quotes, Source: '%s'\n", args[1]);
    printf("After removing quotes, Destination: '%s'\n", args[2]);

    struct stat statbuf;
    if (stat(args[1], &statbuf) == -1) {
        perror("Error checking source file");
        return;
    }

    char finalDestPath[1024];
    if (stat(args[2], &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
        snprintf(finalDestPath, sizeof(finalDestPath), "%s/%s", args[2], basename(args[1]));
    } else {
        strncpy(finalDestPath, args[2], sizeof(finalDestPath));
    }

    printf("Attempting to move to: '%s'\n", finalDestPath);
    if (rename(args[1], finalDestPath) != 0) {
        perror("Error moving file");
    } else {
        printf("Moved '%s' to '%s'\n", args[1], finalDestPath);
    }
}


// This function tokenizes the input string into separate arguments,
// handling quoted arguments correctly. It skips leading and trailing quotes
// and considers the space-separated parts as separate arguments.
void parseInput(char* input, char** args, int maxArgs) {
    int argc = 0;
    // Example: move "test file" "test dir"
    char *next_token = NULL;
    char *p = strtok_r(input, " ", &next_token);
    while (p != NULL && argc < maxArgs - 1) {
        if ((*p == '\"' || *p == '\'') && p[strlen(p) - 1] == *p) { // Quoted argument
            p[strlen(p) - 1] = '\0';  // Remove the last quote
            args[argc++] = p + 1;     // Skip the first quote
        } else {
            args[argc++] = p;
        }
        p = strtok_r(NULL, " ", &next_token);
    }
    args[argc] = NULL;
}


// Appends a given string to a file. If the file does not exist, it is created 
// This function takes an array of strings (`args`) where:
// - `args[1]` contains the string to be appended to the file.
// - `args[2]` specifies the path to the file to which the string should be appended.
void echoppend(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "echoppend: Missing arguments\n");
        return;
    }

    // Find the last argument which should be the file path
    int i = 0;
    while (args[i] != NULL) i++;
    char *filePath = args[i - 1];

    // Open or create the file for appending
    int fd = open(filePath, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1) {
        perror("echoppend");
        return;
    }

    // Concatenate the string arguments to form the message to append
    char message[1024] = {0}; // Assuming a reasonable max length for simplicity
    for (int j = 1; j < i - 1; j++) {
        strcat(message, args[j]);
        if (j < i - 2) strcat(message, " "); // Add spaces between words
    }
    strcat(message, "\n"); // Add a newline at the end of the message

    // Write the message to the file
    if (write(fd, message, strlen(message)) == -1) {
        perror("echoppend write");
    }

    // Close the file descriptor
    close(fd);
}


// Helper function to process arguments and remove surrounding quotes
void processArgument(char **arg) {
    if ((*arg)[0] == '"' || (*arg)[0] == '\'') {
        (*arg)++; // Skip the first quote
    }
    size_t len = strlen(*arg);
    if ((*arg)[len - 1] == '"' || (*arg)[len - 1] == '\'') {
        (*arg)[len - 1] = '\0'; // Remove the last quote
    }
}


// echolite - Overwrites the content of a given file with a specified string. 
// This function simulates the behavior of the Unix 'echo' command with overwrite ('>') redirection.
// It accepts a string to write and a path to a file. If the file exists, its content is replaced with
// the provided string. If the file does not exist, it is created and then written to. This function
// is designed to handle simple text writing scenarios.
void echorite(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "echorite: Missing arguments\n");
        return;
    }

    // Process string and file path to remove possible quotes
    processArgument(&args[1]);
    processArgument(&args[2]);

    // Open (or create) the file for writing, truncating it to zero length
    int fd = open(args[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("echorite");
        return;
    }

    // Write the string to the file
    if (write(fd, args[1], strlen(args[1])) == -1) {
        perror("echorite write");
    }

    // Close the file descriptor
    close(fd);
}


// readFile - Prints the content of a given file to the standard output. 
// This function simulates the basic functionality of the 'cat' Unix command, but without additional
// options. It is designed to open a specified file, read its contents, and print them to stdout.
// This can be used to display text files or other readable data stored in files.
void readFile(char **args) {
    if (args[1] == NULL) {
        printf("readFile: Missing file path\n");
        return;
    }

    int fd = open(args[1], O_RDONLY);
    if (fd == -1) {
        perror("readFile");
        return;
    }

    char buffer[1024];
    ssize_t bytesRead;
    while ((bytesRead = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';  // Ensure null termination
        printf("%s", buffer);
    }

    close(fd);
}



// Calculates and prints the number of lines or words in a file.
// args[1]: Specifies the counting mode ("-l" for lines, "-w" for words).
// args[2]: The file path.
// If the file doesn't exist or the option is incorrect, it prints an error message.
void wordCount(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "wc: Missing option or file name\n");
        return;
    }

    const char *option = args[1];
    const char *filePath = args[2];
    FILE *file = fopen(filePath, "r");
    if (!file) {
        perror("wc");
        return;
    }

    char buffer[1024];
    int lines = 0, words = 0;
    int inWord = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
        char *ptr = buffer;
        while (*ptr) {
            if (*ptr == '\n') {
                lines++;
            }
            if (isspace((unsigned char)*ptr)) {
                inWord = 0;
            } else if (!inWord) {
                inWord = 1;
                words++;
            }
            ptr++;
        }
    }

    if (strcmp(option, "-l") == 0) {
        printf("%d\n", lines);
    } else if (strcmp(option, "-w") == 0) {
        printf("%d\n", words);
    } else {
        fprintf(stderr, "wc: Invalid option\n");
    }

    fclose(file);
}

