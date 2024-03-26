// include/welcome.h
#ifndef WELCOME_H
#define WELCOME_H

// Function to display the welcome message
void displayWelcomeMessage();

// Function to get and display the location (current directory, username, hostname)
void getLocation(void);

// Function to split a string into an array of strings based on spaces
char **splitArgument(char *str);


#endif // WELCOME_H
