#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <baseFunctions.h>

#define FIELD_INFO_INDEX 1

typedef enum {
    STRING,
    INTEGER,
    FLOAT,
    DOUBLE,
} ValueTypes;

typedef struct {
    char *name;
    int type;
    int size;
} Field;

typedef struct {
    FILE *sourceFile;
    Field *fields;
    int offsetSize;
    int amountOfFields;
} Metadata;

/* Inicia cada struct de fields */
void initField(Metadata *metadata) {
    metadata->fields = (Field *) realloc(metadata->fields, metadata->amountOfFields * sizeof(Field));
}

int getFieldType(Metadata *metadata,int fieldIndex, char **tokens) {
    if (strcmp(*tokens, "char") == 0) {
        metadata->fields[fieldIndex].type = STRING;
        metadata->fields[fieldIndex].size = sizeof(char);

        return 0;
    }

    if(strcmp(*tokens, "char") == '[' ) {
        metadata->fields[fieldIndex].type = STRING;

        char *removeBraces = strtok(*tokens, "[]");
        removeBraces = strtok(NULL, "[]");
        metadata->fields[fieldIndex].size = atoi(removeBraces);

        return 0;
    }
    
    if (strcmp(*tokens, "int") == 0 ) {
        metadata->fields[fieldIndex].type = INTEGER;
        metadata->fields[fieldIndex].size = sizeof(int); 

        return 0;
    }

    if (strcmp(*tokens, "float") == 0 ) {
        metadata->fields[fieldIndex].type = FLOAT;
        metadata->fields[fieldIndex].size = sizeof(float);

        return 0;
    }
    
    if(strcmp(*tokens, "double") == 0) {
        metadata->fields[fieldIndex].type = DOUBLE;
        metadata->fields[fieldIndex].size = sizeof(double);

        return 0;

    }

    return -1; //novamente, caso haja um tipo desconhecido no arquivo de metadados    
}

/* Função que inicializa cada elemento da Metadata */
Metadata *initMetadata(FILE *metadataFile) {
    Metadata *metadata = (Metadata *) malloc(sizeof(Metadata));

    metadata->offsetSize = 0;
    metadata->fields = NULL;
    metadata->amountOfFields = 0;

    return metadata;
}

/* Função que pega todas as informações do arquivo de metadata */
Metadata *getMetadata(FILE *metadataFile) {
    Metadata *metadata = initMetadata(metadataFile);
    char *line;
    char **tokens;
    int amountOfTokens;

    for (int i = -1; (line = readLine(metadataFile)) != NULL; ++i) {
        /* Depois de ler o nome do arquivo, ele irá ler o par de informações: nome do campo e tipo, nessa ordem */
        if (i == -1) {
            tokens = tokenfy(line, " ", ".", &amountOfTokens); //pega o nome do arquivo retirando a extensão
            free(line); //Dá free na linha pois tudo nela já foi salvo individualmente em tokens
            char *tmpName = strdup(  strcat(tokens[FIELD_INFO_INDEX], ".idx")); // Já coloca a extensão certa
            metadata->sourceFile = fopen(tmpName, "w+");
            free(tmpName);

            if (!metadata->sourceFile) { 
                perror("Error opening the database sourceFile\n");
                exit;
            }

            freeTokens(tokens, amountOfTokens); 
        }

        else {
            metadata->amountOfFields++;
            initField(metadata);

            /* Salva o nome do campo */
            tokens = tokenfy(line, " ", NULL, &amountOfTokens);
            free(line);
            metadata->fields[i].name = getString(&tokens[FIELD_INFO_INDEX]);
            freeTokens(tokens, amountOfTokens);   
    
            /* Salva o tipo do campo */
            line = readLine(metadataFile);
            tokens = tokenfy(line, " ", NULL, &amountOfTokens);
            free(line);
        
            if (getFieldType(metadata, i, &tokens[FIELD_INFO_INDEX]) == -1) { //Pega o tipo e o tamanho do field
                perror("Error reading field type: not a valid field\n");
            }

            metadata->offsetSize += metadata->fields[i].size; // Vai adicionando cada tamanho ao offset total de cada dado
            freeTokens(tokens, amountOfTokens);   
        }
    }

    return metadata;
}

/* Daá free em tudo relacionado ao metadata */
void freeMetadata(Metadata *metadata) {
    for(int i = 0; i < metadata->amountOfFields; ++i) {
        free(metadata->fields[i].name);
    }

    fclose(metadata->sourceFile);
    free(metadata->fields);
    free(metadata);
}
