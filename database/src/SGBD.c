#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <commands.h>

#define FALSE 0
#define TRUE 1
#define COMMAND_INDEX 0
#define TYPE_KEY_INDEX 0

int main(int argc, char *argv[]) {
    Database *database = initDatabase();
    int amountOfTokens;
    char *line;
    char **tokens;
    int *sortedIndexes;

    int hascreatedIndex = FALSE;
    while(strcmp( (line = readLine(stdin) ), "exit") != 0) {

        tokens = tokenfy(line, " ", ", ", &amountOfTokens);
        free(line);

        if (strcmp(tokens[COMMAND_INDEX], "insert") == 0) insertFieldValues(database, tokens);

        else if (hascreatedIndex == FALSE) { // Ou seja, será executado quando o comando não for mais insert e o hascreatedIndx for falso

            //TO-DO: mudar o nome do sortedDataKeys para dataKeys
            sortedIndexes = bubbleSortIndexes(database->sortedDataKeys, database->amountOfData);
            hascreatedIndex = TRUE;
        }

        if (strcmp(tokens[COMMAND_INDEX], "search") == 0) {
            searchIndex(database->metadata->sourceFile, database, sortedIndexes, atoi(tokens[1]));
        }

        freeTokens(tokens, amountOfTokens);
    }

    free(line);
    free(sortedIndexes);
    freeDatabase(database); 
}
