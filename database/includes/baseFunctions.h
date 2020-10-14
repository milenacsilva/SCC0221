#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define READLINE_BUFFER 4096
#define getDelimIndex(x,y) strlen(x) - strlen(y)

char* readLine(FILE* stream);
char **tokenfy(char *line, char *delim1, char *delim2, int *amountOfTokens);
char* getString(char **token);
long getInteger(char **token);
double getFloatingPoint(char **token);
void freeTokens(char **tokens, int amountOfTokens);