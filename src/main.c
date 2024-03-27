    #include <unistd.h>
    #include "episodeA.h"
    #include "episodeB.h"
    #include "episodeC.h"
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h> 


    int main() {
        displayWelcomeMessage();
     //   -------SplitArgument TEST-------
        // char input[] = "cp file1.txt file2.txt";
        // char **arguments = splitArgument(input);

        // if (arguments) {
        //     for (int i = 0; arguments[i] != NULL; i++) {
        //         printf("Argument %d: %s\n", i, arguments[i]);
        //     }

            
        //     int i;
        //     for (i = 0; arguments[i] != NULL; i++) {
        //         free(arguments[i]); 
        //     }
        //     free(arguments); 
        // }
    char *inputBuffer;
     //   char cwd[1024]; // Buffer to store the current working directory
        size_t bufsize = 1024;
        
        while (1) {
            getLocation();
            printf("\nEnter command ('exit' to logout): ");

            inputBuffer = (char *)malloc(bufsize * sizeof(char)); // Allocate input buffer
            if (!inputBuffer) {
                perror("Unable to allocate input buffer");
                exit(EXIT_FAILURE);
            }

            // Get input from the user
            if (getline(&inputBuffer, &bufsize, stdin) == -1) {
                printf("\nRead error or EOF detected. Exiting.\n");
                free(inputBuffer);
                continue;
            }

            // Remove newline character
            inputBuffer[strcspn(inputBuffer, "\r\n")] = 0;
            char* trimmedInput = trimSpaces(inputBuffer); // Apply trimming to handle leading/trailing spaces
            
            // Tokenization for command handling
            char *args[1024]; 
            int i = 0;
            char* token = strtok(trimmedInput, " ");
            while (token != NULL) {
                args[i++] = token;
                token = strtok(NULL, " ");
            }
            args[i] = NULL; 



          
             if (strcmp(args[0], "exit") == 0) {
                printf("\nThank you for using my Shell, you have been Logout successfully!\n");
                if (args[1] != NULL) {
                    logout(args[1]);
                } else {
                    logout(NULL); // Or, adjust 'logout' to handle no arguments
                }
                // The program will exit after 'logout', so no further code will execute
            }
            else if (strcmp(args[0], "cd") == 0) {
                cd(args); // Pass the parsed arguments to cd
            } else if (strcmp(args[0], "cp") == 0 && i >= 3) { // Check for 'cp' command and ensure enough arguments
                cp(args); // Pass the parsed arguments to cp
            } else if (strcmp(args[0], "delete") == 0) { // Use the new function name
                delete(args);
            }
            else if (strstr(inputBuffer, "|")) {
            prepareAndExecutePipe(inputBuffer); // Directly pass the whole command for piping
            }
             else {
                printf("Command not recognized. Use 'exit' to logout, 'cd' to change directory, 'cp' to copy files, or 'delete' to delete files.\n");
            }

            free(inputBuffer); // Free the input buffer at the end of each iteration
        }


        return 0; 
    }

