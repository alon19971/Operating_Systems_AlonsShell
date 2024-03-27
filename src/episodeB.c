    #include "episodeB.h"
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    #include <ctype.h> 

                // ----------EPISODE-B FUNCTIONS----------

    // Trims leading and trailing spaces from a given string.
    //  * This function iterates over the characters of the input string and removes any leading spaces 
    //  * by advancing the pointer to the first non-space character. It then finds the end of the string 
    //  * and moves backwards to remove trailing spaces by placing a new null terminator (`\0`) at the 
    //  * position where trailing spaces start.
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



    // Exits the shell program after printing a farewell message
    // This function checks if the input command is "exit" and, if so, prints a thank you message before exiting the shell.
    // It ensures a graceful exit from the shell by using exit(EXIT_SUCCESS), indicating successful termination.
    void logout(char *input) {
        if (input != NULL && strcmp(input, "exit") == 0) {
            puts("Thank you to use my shell, you have been logout successfully!");
        }
        exit(EXIT_SUCCESS); // Exit the shell successfully
    }



    //   This function handles changing the shell's current directory to the specified path. It supports:
    //   - Simple paths without spaces.
    //   - Paths with spaces by concatenating multiple arguments.
    //   - Paths enclosed in quotes (either single or double quotes) to treat spaces within quotes as part of a single argument.
    void cd(char **arg) {
        char path[1024] = {0}; // Buffer to hold the final path

        if (arg[1] == NULL) {
            puts("-myShell: cd: expected argument to \"cd\"");
            return;
        }

        if (arg[1][0] == '\"' || arg[1][0] == '\'') { // Check if path starts with a quote
            // Assume that quotes are properly closed in the input
            int i = 1;
            while (arg[i] != NULL) {
                // Remove leading quote for first argument
                char* start = (i == 1) ? arg[i] + 1 : arg[i];
                char* end = strstr(start, "\"") ? strstr(start, "\"") : strstr(start, "\'");
                
                if (end) *end = '\0'; // Remove trailing quote
                strcat(path, start);

                if (end || arg[i + 1] == NULL) break; // Stop if end quote found or no more arguments
                
                strcat(path, " "); // Add space between parts
                i++;
            }
        } else if (arg[2] != NULL) { // Multiple arguments, not enclosed in quotes
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



    // Copies the contents of one file to another.
    // This function takes two file paths as input: the source file to copy from, and the destination file to copy to.
    // It opens the source file for reading and the destination file for writing. If either file cannot be opened, 
    // it reports an error. It then reads the contents of the source file character by character and writes each 
    // character to the destination file until reaching the end of the source file (EOF).
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

