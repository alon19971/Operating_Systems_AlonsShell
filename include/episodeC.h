#ifndef EPISODEC_H
#define EPISODEC_H

void delete(char **path);
void mypipe(char **argv1, char **argv2);
void prepareAndExecutePipe(char *input);
        // LS, CAT, ECHO
void listDirectoryContents(char **args);
void displayFileContents(char **args);
void echo(char **args);


#endif // EPISODEC_H
