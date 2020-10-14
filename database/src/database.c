#include <stdio.h>
#include <stdlib.h>
#include <metadata.h>

typedef struct {
    FILE *metadataFile;
    Metadata *metadata;
    int *sortedDataKeys;
    int amountOfData;
} Database;

/* Iniciliza os elementos do banco de dados */
Database *initDatabase() {
    Database *database = (Database *) malloc(sizeof(Database));

    char *metadataFileName = readLine(stdin);
    database->metadataFile = fopen(metadataFileName, "r+");
    free(metadataFileName);

    if(!database->metadataFile) {
        perror("Error opening metadataFile\n");
        exit(-1);
    }

    database->metadata = getMetadata(database->metadataFile);

    database->amountOfData = 0;

    database->sortedDataKeys = NULL;

    return database;
}

/* Funções que juntas funcionam pra ordernar os dados, mais especificamente os códigos desses */
void swap(int *a, int *b){
    int tmp = *a;
    *a = *b;
    *b = tmp;
} 

/* Em vez de ordenar cada campo, essa função cria um vetor de indexes, ordena ele a partir da comparação das keys e o retorna */
int *bubbleSortIndexes(int *arr, int amountOfData) {
    int *indexes = (int* ) malloc(sizeof(int) * amountOfData);
    for (int i = 0; i < amountOfData; ++i) indexes[i] = i; // Prenche cada index com o numero de 0 a amountOfdata-1   

    for (int j = 0; j < amountOfData; j++) {
        for (int i = 0; i < amountOfData - 1; i++) {
            if (arr[indexes[i]] > arr[indexes[i+1]]) {
                swap(&indexes[i], &indexes[i+1]);
            }
         }
    }

    return indexes;

}
/* Da free em toda database */
void freeDatabase(Database *database) {
    fclose(database->metadataFile);

    freeMetadata(database->metadata);

    free(database->sortedDataKeys);
    free(database);
}
