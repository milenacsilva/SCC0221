#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <database.h>

#define COMAND_POS_IN_LINE 1 // Isso significa que no inicio de uma linha tem um comando específico

void insertFieldValues(Database *database, char **tokens);
int searchKey(Database *database, int *sortedIndexes, int key, int min, int max);
void searchIndex(FILE *fp, Database *database, int *sortedIndexes, int key);
