#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>  
#include <limits.h> 
#include <sys/types.h>
#include <pwd.h>

#define BUFF_SIZE PATH_MAX


// Define the welcome function when openning the shell
void displayWelcomeMessage() {
    printf("\n");
    printf("       /\\  \n");
    printf("      /  \\      _-'\n");
    printf("    /\\   \\    _-'\n");
    printf("   /  \\   \\ _-'\n");
    printf("  / /\\ \\  \\     Alon's Shell\n");
    printf(" / /  \\ \\  \\    Where cleverness meets efficiency.\n");
    printf("/_/    \\_\\__\\   Type 'help' for a list of commands.\n");
    printf("\n");
    printf("Version 1.0.0 - Embrace the spirit of the fox.\n");
    printf("\n");
    printf("Explore more: https://github.com/alon19971\n");
    printf("\n");
    printf("\"Be like the fox who makes more tracks than necessary, some in the wrong direction. Practice resilience.\"\n");
    printf("\n");
}


// ANSI Escape Codes for formatting
void bold() {
    printf("\033[1m");
}

void blue() {
    printf("\033[34m");
}

void green() {
    printf("\033[32m");
}

void reset() {
    printf("\033[0m");
}

// A function that returns the path of
// the current terminal exactly like the original
void getLocation() {
    char location[BUFF_SIZE];
    char hostname[HOST_NAME_MAX];
    struct passwd *pw;
    uid_t uid;

    uid = getuid();
    pw = getpwuid(uid);

    if (pw == NULL)
    {
        perror("Failed to get username");
        return;
    }

    if (gethostname(hostname, sizeof(hostname)) == -1)
    {
        perror("gethostname() error");
        return;
    }

    if (getcwd(location, BUFF_SIZE) == NULL)
    {
        puts("Error retrieving current directory");
    }
    else
    {
        bold();
        green();
        printf("%s@", pw->pw_name);
        printf("%s:", hostname);
        blue();
        printf("%s", location);
        reset();
        printf("$ ");
    }
}


char **splitArgument(char *str) {
    char *subStr;
    int size = 1; // Start with 1 for the first token
    int index = 0;
    subStr = strtok(str, " ");
    char **arguments = (char **)malloc(size * sizeof(char *));
    if (!arguments) {
        perror("Malloc failed");
        return NULL;
    }

    while (subStr != NULL) {
        arguments[index++] = subStr;
        subStr = strtok(NULL, " ");
        if (subStr != NULL) { // Only resize if there are more tokens
            size++;
            char **temp = (char **)realloc(arguments, size * sizeof(char *));
            if (!temp) {
                perror("Realloc failed");
                free(arguments);
                return NULL;
            }
            arguments = temp;
        }
    }
    arguments[index] = NULL; // Correctly assign NULL after the last real token

    return arguments;
}


