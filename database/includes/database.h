#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <metadata.h>

typedef struct {
    FILE *metadataFile;
    Metadata *metadata;
    int *sortedDataKeys;
    int amountOfData;
} Database;

Database *initDatabase();
void freeDatabase(Database *database);


//TO-DO: substituir esses dois por qsort :)
void swap(int *a, int *b);
int *bubbleSortIndexes(int *arr, int amountOfData);
