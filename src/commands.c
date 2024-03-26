#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h> 

// Implement trimSpaces in commands.c
char* trimSpaces(char* input) {
    while(isspace((unsigned char)*input)) input++; 

    if(*input == 0)  
        return input;

    char* end = input + strlen(input) - 1;
    while(end > input && isspace((unsigned char)*end)) end--;

    // Write new null terminator character
    end[1] = '\0';

    return input;
}


// Function to handle Logout from the shell
// by ignoring space while applying 'exit' input
void logout(char *input) {
    if (input != NULL && strcmp(input, "exit") == 0) {
        puts("Thank you to use my shell, you have been logout successfully!");
    }
    exit(EXIT_SUCCESS); // Exit the shell successfully
}


// 
void cd(char **arg) {
    char path[1024] = {0}; // Buffer to hold the final path

    if (arg[1] == NULL) {
        puts("-myShell: cd: expected argument to \"cd\"");
        return;
    } else if (arg[2] != NULL) { // Multiple arguments may imply a path with spaces
        // Attempt to concatenate arguments to form the path
        for (int i = 1; arg[i] != NULL; i++) {
            strcat(path, arg[i]);
            if (arg[i+1] != NULL) strcat(path, " "); // Add space between parts
        }
    } else {
        // Single argument - simple path
        strcpy(path, arg[1]);
    }

    // Try to change directory
    if (chdir(path) != 0) {
        printf("-myShell: cd: %s: No such file or directory\n", path);
    }
}


// Function to handle file paths with spaces
void cp(char **arguments) {
    char ch;
    FILE *src, *des;
    if ((src = fopen(arguments[1], "r")) == NULL) {
        printf("Error opening source file: %s\n", arguments[1]);
        return;
    }
    if ((des = fopen(arguments[2], "w")) == NULL) {
        printf("Error opening destination file: %s\n", arguments[2]);
        fclose(src); // Ensure the source file is closed
        return;
    }

    while ((ch = fgetc(src)) != EOF) {
        fputc(ch, des);
    }

    fclose(src);
    fclose(des);
    printf("File copied successfully from %s to %s\n", arguments[1], arguments[2]);
}

