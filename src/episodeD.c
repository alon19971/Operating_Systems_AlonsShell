#include "episodeD.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

                     
                // --------EPISODE D FUNCTIONS--------


// Removes leading and trailing quotes from a string.
// This function modifies the string in place by adjusting the pointer
// to skip leading quotes and null-terminating the string early to remove trailing quotes.
// It handles both single ('') and double ("") quotes.
void removeQuotes(char **str) {
    char *p = *str;
    size_t len = strlen(p);

    // If the string starts and ends with quotes
    if ((p[0] == '"' || p[0] == '\'') && (p[len - 1] == '"' || p[len - 1] == '\'')) {
        p[len - 1] = '\0';  // Null terminate one character early to remove ending quote
        ++p;  // Increment pointer to skip leading quote
    }

    *str = p;  // Update the caller's pointer
}


// Moves a file from a source path to a destination path.
// This function supports moving files into directories and renaming files.
// It handles paths with spaces by requiring paths to be passed with quotes if spaces are present.
void move(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        printf("move: missing file operand\n");
        return;
    }

    // Remove quotes from paths if present
    removeQuotes(&args[1]);
    removeQuotes(&args[2]);

    char *sourcePath = args[1];
    char *destinationPath = args[2];
    struct stat statbuf;
    
    // Check if destination is a directory
    if (stat(destinationPath, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
        char finalDestPath[1024];
        char *fileName = strrchr(sourcePath, '/');
        fileName = fileName ? fileName + 1 : sourcePath;  // Get the base name of the file

        snprintf(finalDestPath, sizeof(finalDestPath), "%s/%s", destinationPath, fileName);

        if (rename(sourcePath, finalDestPath) != 0) {
            perror("Error moving file");
            return;
        }
    } else {
        if (rename(sourcePath, destinationPath) != 0) {
            perror("Error moving file");
            return;
        }
    }

    printf("Moved '%s' to '%s'\n", sourcePath, destinationPath);
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

