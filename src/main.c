#include "welcome.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 


int main() {
    getLocation();
    displayWelcomeMessage();

    char input[] = "cp file1.txt file2.txt";
    char **arguments = splitArgument(input);

    if (arguments) {
        for (int i = 0; arguments[i] != NULL; i++) {
            printf("Argument %d: %s\n", i, arguments[i]);
        }

        // Remember to free each string in the array, then the array itself
        int i;
        for (i = 0; arguments[i] != NULL; i++) {
            free(arguments[i]); // Free each string
        }
        free(arguments); // Free the array of pointers
    }

    return 0;
}
