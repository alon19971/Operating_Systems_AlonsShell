/**
 * @file main.c
 * @brief A simple shell program implementation.
 * 
 * This file contains the main entry point and logic for a simple shell program.
 * It handles user commands and executes corresponding functions.
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

// Headers
#include "episodeA.h"
#include "episodeB.h"
#include "episodeC.h"
#include "episodeD.h"

/**
 * @brief The main entry point of the program.
 * 
 * This function serves as the main entry point of the program. It displays a welcome
 * message, prompts the user for input, processes user commands, and executes
 * corresponding functions. It continues to run until the user enters the 'exit' command.
 * 
 * @return 0 on successful execution, non-zero otherwise.
 */
int main() {
    displayWelcomeMessage();

    // Allocate memory for input buffer
    size_t bufsize = 1024;
    char* inputBuffer = (char *)malloc(bufsize * sizeof(char));
    if (!inputBuffer) {
        perror("Unable to allocate input buffer");
        return EXIT_FAILURE;
    }

    // Main shell loop
    while (1) {
        getLocation();
        printf("\nEnter command ('exit' to logout): ");

        // Get input from the user
        if (getline(&inputBuffer, &bufsize, stdin) == -1) {
            printf("\nRead error or EOF detected. Exiting.\n");
            break;
        }

        // Remove newline character
        inputBuffer[strcspn(inputBuffer, "\r\n")] = '\0';

        // Trim leading and trailing spaces
        char* trimmedInput = trimSpaces(inputBuffer);

        // Tokenize input into arguments
        char* args[1024];
        memset(args, 0, sizeof(args));
        int i = 0;
        char* token = strtok(trimmedInput, " ");
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }

        // Handling of various commands
        if (strcmp(args[0], "exit") == 0) {
            printf("\nThank you for using my Shell, you have been Logout successfully!\n");
            break; // Exit the loop to end the program
        }
        else if (strcmp(args[0], "ls") == 0) {
            listDirectoryContents(args);
        } 
        else if (strcmp(args[0], "cat") == 0) {
            displayFileContents(args);
        }
        else if (strcmp(args[0], "echo") == 0) {
            echo(args);
        }
        else if (strcmp(args[0], "cd") == 0) {
            cd(args);
        }
        else if (strcmp(args[0], "cp") == 0) {
            cp(args);
        }
        else if (strcmp(args[0], "delete") == 0) {
            delete(args);
        }
        else if (strcmp(args[0], "move") == 0) {
            move(args);
        }
        else if (strstr(inputBuffer, " >> ")) {
            echoppend(args); 
        }
        else if (strstr(inputBuffer, ">")) {
            echorite(args);
        }
        else if (strcmp(args[0], "read") == 0) {
            readFile(args);
        }
        else if (strcmp(args[0], "wc") == 0) {
            wordCount(args);
        } 
        else {
            printf("Command not recognized. Use 'exit' to logout, 'cd' to change directory, 'cp' to copy files, or 'delete' to delete files.\n");
        }
    }

    free(inputBuffer); // Free the input buffer at the end
    return 0;
}
