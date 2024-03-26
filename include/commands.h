#ifndef COMMANDS_H
#define COMMANDS_H

char* trimSpaces(char* input); // Trims leading and trailing spaces from a string
void logout(char *input); // Performs the logout operation
void cd(char **arg); // Changes the current working directory
void cp(char **arguments); // Copies a file from a source to a destination

#endif // COMMANDS_H
