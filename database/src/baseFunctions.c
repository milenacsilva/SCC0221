#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define READLINE_BUFFER 4096
#define getDelimIndex(x,y) strlen(x) - strlen(y)

char* readLine(FILE* stream) {
    char *str = NULL;
    int i = 0;

    if (feof(stream))  
        return NULL;

    do {
        if (i % READLINE_BUFFER == 0) {
            int j = (i / READLINE_BUFFER + 1) * READLINE_BUFFER;
            str = (char* ) realloc(str, sizeof(char ) * j);
        }
        str[i] = fgetc(stream);
        ++i;
    } while (str[i-1] != '\n' && str[i-1] != EOF);

    if (i <= 1) return NULL;   // No caso se ter um \n no final da última linha do input

    str = (char* ) realloc(str, sizeof(char ) * i); //Elimina o espaço adicional alocado
    str[i-1] = '\0'; //Torna em uma string
    
    return str;
}

char **tokenfy(char *line, char *delim1, char *delim2, int *amountOfTokens) {
    char **tokens = NULL;
    char *str;
    char *delim = delim1;
    int i,j;

    for(i = 0, j = 0; (str = strstr(&line[i], delim)) != NULL; ++j) {
        tokens = (char **) realloc(tokens, sizeof(char* ) * (j + 1));
        tokens[j] = strndup(&line[i], getDelimIndex(line, str) - i);

        i = getDelimIndex(line, str) + strlen(delim);
        delim = (delim2 != NULL)? delim2: delim1;
    }

    tokens = (char **) realloc(tokens, sizeof(char* ) * (j + 1));
    tokens[j] = strdup(&line[i]);

    *amountOfTokens = j + 1;
    free(str);

    return tokens;

}

char* getString(char **token) {
    char *removeQuotes = strtok(*token,"\""); // This is to remove the double that comes along with a string type
    return strdup(removeQuotes);
    
}

long getInteger(char **token) {
    return atol(*token);
}

double getFloatingPoint(char **token) {
    return atof(*token);
}

void freeTokens(char **tokens, int amountOfTokens) {
    for (int i = 0; i < amountOfTokens; ++i) {
        free(tokens[i]);
    }
    free(tokens);
}