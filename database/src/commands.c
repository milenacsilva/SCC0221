#include <stdio.h>
#include <stdlib.h>
#include <database.h>
#include <string.h>

#define COMAND_POS_IN_LINE 1 // Isso significa que no inicio de uma linha tem um comando específico


void insertFieldValues(Database *database, char **tokens) { 
    fseek(database->metadata->sourceFile, database->amountOfData * database->metadata->offsetSize, SEEK_SET);
    ++database->amountOfData;

    for(int i = 0; i < database->metadata->amountOfFields; ++i) { //Considerando que o i = 0 é na real o comando insert
        void *tmpPointer;
        if (database->metadata->fields[i].type == STRING) {
            tmpPointer = getString(&tokens[i + COMAND_POS_IN_LINE]);
            fwrite(tmpPointer, sizeof(char), database->metadata->fields[i].size, database->metadata->sourceFile);
            free(tmpPointer);
        }
        else if (database->metadata->fields[i].type == INTEGER) { 
            int a = getInteger(&tokens[i + COMAND_POS_IN_LINE]);
            if (i == 0) {
                database->sortedDataKeys = (int *) realloc(database->sortedDataKeys, sizeof(int) * database->amountOfData);
                database->sortedDataKeys[database->amountOfData - 1] = a;
            }
            tmpPointer = &a;
            fwrite(tmpPointer, sizeof(int), 1,  database->metadata->sourceFile);
        }
        else if (database->metadata->fields[i].type == FLOAT) {
            float f = getFloatingPoint(&tokens[i + COMAND_POS_IN_LINE]);
            fwrite(tmpPointer, sizeof(float), 1, database->metadata->sourceFile);
        }
        else if (database->metadata->fields[i].type == DOUBLE) { 
            double d = getFloatingPoint(&tokens[i + COMAND_POS_IN_LINE]);
            tmpPointer = &d;
            fwrite(tmpPointer, sizeof(double), 1, database->metadata->sourceFile);   
        }
    }
}

int searchKey(Database *database,int *sortedIndexes, int key, int min, int max) {
    if (max >= min) {
        int mid = min + ((max - min) / 2);
    
        if (database->sortedDataKeys[sortedIndexes[mid]] == key) {
            return sortedIndexes[mid];  
        }

        if(database->sortedDataKeys[sortedIndexes[mid]] > key){
            return searchKey(database, sortedIndexes, key, min, mid - 1);
        }

        return searchKey(database, sortedIndexes, key, mid + 1, max);

    }
    
    return -1;   
}

void searchIndex(FILE *fp, Database *database, int *sortedIndexes, int key) {
    int index = searchKey(database, sortedIndexes, key, 0, database->amountOfData - 1);
    
    if (index == -1) {
        perror("Key not found\n");
        return;
    }

    fseek(fp, index * database->metadata->offsetSize, SEEK_SET);

    for(int i = 0; i < database->metadata->amountOfFields; ++i) {
            printf("%s: ", database->metadata->fields[i].name);
            if (database->metadata->fields[i].type == STRING) {
                char *pointer = malloc(database->metadata->fields[i].size);
                fread(pointer,sizeof(char), database->metadata->fields[i].size, fp);
                printf("%s\n", pointer);
                free(pointer);
            }

            if (database->metadata->fields[i].type == INTEGER) {
                int *pointer = malloc(database->metadata->fields[i].size);
                fread(pointer,sizeof(int), 1, fp);
                printf("%i\n", pointer[0]);
                free(pointer);
            }

            if (database->metadata->fields[i].type == FLOAT) {
                float *pointer = malloc(database->metadata->fields[i].size);
                fread(pointer,sizeof(float), 1, fp);
                printf("%.2lf\n", pointer[0]);
                free(pointer);
            }

            if (database->metadata->fields[i].type == DOUBLE) {
                double *pointer = malloc(database->metadata->fields[i].size);
                fread(pointer,sizeof(double), 1, fp);
                printf("%.2lf\n", pointer[0]);
                free(pointer);
            }
    }
    putchar('\n');
}
