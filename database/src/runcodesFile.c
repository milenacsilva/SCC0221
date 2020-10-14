#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define READLINE_BUFFER 4096

#define FIELD_INFO_INDEX 1
#define KEY_FIELD_INDEX 0 
#define COMAND_POS_IN_LINE 1 
#define COMAND_INDEX_IN_LINE 0 

#define FALSE 0
#define TRUE 1

/* Enumera os tipos para ficar mais prático de processa-los */
typedef enum _types {
    STRING,
    INTEGER,
    FLOAT,
    DOUBLE,
} ValueTypes; 

typedef struct _field {
    char *name;
    short type;
    int size;
} Field;

typedef struct _metadata {
    FILE *metadataFile;
    Field *fields;
    int offsetSize;
    int amountOfFields;
} Metadata;

typedef struct _database {
    FILE *sourceFile;
    Metadata *metadata;
    int *dataKeys;
    int amountOfData;
} Database;

/* Função que lê uma linha de texto */
char* readLine(FILE* stream) {
    char *str = NULL;

    if (feof(stream)) return NULL; //Quebra o loop da main

    int i = 0;
    do {
        if (i % READLINE_BUFFER == 0) {
            int j = (i / READLINE_BUFFER + 1) * READLINE_BUFFER;
            str = (char*) realloc(str, sizeof(char) * j);
        }

        str[i] = fgetc(stream);
        ++i;
    } while (str[i - 1] != '\n' && str[i - 1] != EOF);

    if (i <= 2) { // No caso se ter um \n no final da última linha do input tbm quebra o loop da main
	    free(str);
 	    return NULL;   
    }

    if (str[i - 2] == '\r') { // Caso tenha o \r no final antes do \n
        str[i - 2] = '\0';
        str = (char*) realloc(str, i - 1);

    } else {
        str[i-1] = '\0';
        str = (char*) realloc(str, i);
    }

    return str;
}

/* Função semelhante a strtok da string.h só que em vez de operar sobre delimitadores do tipo char,
 opera sobre o tipo char*. Além disso, podem ser colocados 2 delimitadores: se o segundo for 
 diferente de NULL, o primeiro só será usado uma vez e o segundo quantas vezes mais necessárias */
char **tokenfy(char *line, char *delim1, char *delim2, int *amountOfTokens) {
    char **tokens = NULL;
    char *delim = delim1;
    char *tmpStr;
    int i,j;

    for(i = 0, j = 0; (tmpStr = strstr(&line[i], delim)) != NULL; ++j) {
        tokens = (char**) realloc(tokens, sizeof(char*) * (j + 1));
        tokens[j] = strndup(&line[i], strlen(line) - strlen(tmpStr) - i);

        i = strlen(line) - strlen(tmpStr) + strlen(delim);
        delim = (delim2 != NULL)? delim2: delim1;
    }

    tokens = (char **) realloc(tokens, sizeof(char* ) * (j + 1));
    tokens[j] = strdup(&line[i]);

    *amountOfTokens = j + 1;
    return tokens;
}

char* getString(char **token) { //Retira as aspas e fz a copia de uma string em tokens
    char *removeQuotes = strtok(*token,"\""); 
    return strdup(removeQuotes);   
}

void freeTokens(char **tokens, int amountOfTokens) { //Função para dar free nos tokens da tokenfy
    for (int i = 0; i < amountOfTokens; ++i) 
        free(tokens[i]);

    free(tokens);
}

/* Acha o tipo e o tamanho de cada field a partir de comparações */
int getFieldType(Metadata *metadata,int fieldIndex, char **tokens) {
    if (strcmp(*tokens, "char") == 0) {
        metadata->fields[fieldIndex].type = STRING; // Para evitar o uso de strcmp, números são dados para cada tipo
        metadata->fields[fieldIndex].size = sizeof(char);

        return 0;
    }

    if(strcmp(*tokens, "char") == '[' ) { 
        metadata->fields[fieldIndex].type = STRING;

        char *removeBraces = strtok(*tokens, "[]"); // Retira o número de dentro dos cochetes
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

    return -1; //caso haja um tipo desconhecido no arquivo de metadados    
}

/* Função que inicializa cada elemento da Metadata */
Metadata *initMetadata(FILE *metadataFile) {
    Metadata *metadata = (Metadata *) malloc(sizeof(Metadata));

    metadata->metadataFile = metadataFile;
    metadata->offsetSize = 0;
    metadata->fields = NULL;
    metadata->amountOfFields = 0;

    return metadata;
}

/* Função que pega todas as informações do arquivo de metadata */
void getMetadata(Database *database) {
    char *line;
    char **tokens;
    int amountOfTokens;

    for (int i = -1; (line = readLine(database->metadata->metadataFile)) != NULL; ++i) {
    
        if (i == -1) { //Primeiro lê-se o nome do arquivo
            tokens = tokenfy(line, " ", ".", &amountOfTokens); //pega o nome do arquivo retirando a extensão
            free(line);

            char *tmpName = strdup(strcat(tokens[FIELD_INFO_INDEX], ".idx")); // Já coloca a extensão certa
    
            if (!(database->sourceFile = fopen(tmpName, "w+"))) { //Caso de algum problema em abrir o arquivo
                perror("Error opening the database sourceFile\n");
                exit(0);
            }

            free(tmpName);
            freeTokens(tokens, amountOfTokens); 
        }

        else { //Depois, lê-se o par de informação
            database->metadata->amountOfFields++;
            database->metadata->fields = (Field *) realloc(database->metadata->fields, database->metadata->amountOfFields * sizeof(Field));

            /* Salva o nome do campo */
            tokens = tokenfy(line, " ", NULL, &amountOfTokens);
            free(line);
            database->metadata->fields[i].name = getString(&tokens[FIELD_INFO_INDEX]);
            freeTokens(tokens, amountOfTokens);   
    
            /* Salva o tipo do campo */
            line = readLine(database->metadata->metadataFile);
            tokens = tokenfy(line, " ", NULL, &amountOfTokens);
            free(line);
        
            if (getFieldType(database->metadata, i, &tokens[FIELD_INFO_INDEX]) == -1) { //Pega o tipo e o tamanho do field
                perror("Error reading field type: not a valid field\n");
            }

            database->metadata->offsetSize += database->metadata->fields[i].size; // Vai adicionando cada tamanho ao offset total de cada dado
            freeTokens(tokens, amountOfTokens);   
        }
    }  
}

/* Daá free em tudo relacionado ao metadata */
void freeMetadata(Metadata *metadata) {
    for(int i = 0; i < metadata->amountOfFields; ++i) {
        free(metadata->fields[i].name);
    }

    fclose(metadata->metadataFile);
    free(metadata->fields);
    free(metadata);
}

/* Iniciliza os elementos do banco de dados */
Database *initDatabase() {
    Database *database = (Database *) malloc(sizeof(Database));

    char *metadataFileName = readLine(stdin);
    FILE *metadataFile = fopen(metadataFileName, "r+");
    free(metadataFileName);

    if(!metadataFile) { //Caso de erro na abertura do arquivo
        perror("Error opening metadataFile: not a valid file name\n");
        exit(-1);
    }

    database->metadata = initMetadata(metadataFile);
    getMetadata(database);

    database->amountOfData = 0;
    database->dataKeys = NULL;

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

    for (int i = 0; i < amountOfData; ++i) 
        indexes[i] = i; // Prenche cada index com o numero de 0 a amountOfdata-1   

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
    fclose(database->sourceFile);
    freeMetadata(database->metadata);
    free(database->dataKeys);
    free(database);
}

/* Essa função vai escrevendo o arquivo binário conforme vai lendo as linhas na main */
void insertFieldValues(Database *database, char **tokens) { 
    fseek(database->sourceFile, database->amountOfData * database->metadata->offsetSize, SEEK_SET);
    ++database->amountOfData;

    for(int i = 0; i < database->metadata->amountOfFields; ++i) { //Considerando que o i = 0 é na real o comando insert
     
        if (database->metadata->fields[i].type == STRING) { // Se for uma string ou um char
            void *tmpCharPointer = getString(&tokens[i + COMAND_POS_IN_LINE]);
            fwrite(tmpCharPointer, sizeof(char), database->metadata->fields[i].size, database->sourceFile);
            free(tmpCharPointer);
        }

        else if (database->metadata->fields[i].type == INTEGER) { // Se for um inteiro
            int tmpInt = atol(tokens[i + COMAND_POS_IN_LINE]);
            fwrite(&tmpInt, sizeof(int), 1,  database->sourceFile);

            if (i == KEY_FIELD_INDEX) { 
                database->dataKeys = (int *) realloc(database->dataKeys, sizeof(int) * database->amountOfData);
                database->dataKeys[database->amountOfData - 1] = tmpInt;
            }
        }

        else if (database->metadata->fields[i].type == FLOAT) { // Se for um float
            float tmpFloat = atof(tokens[i + COMAND_POS_IN_LINE]);
            fwrite(&tmpFloat, sizeof(float), 1, database->sourceFile);
        }

        else if (database->metadata->fields[i].type == DOUBLE) {  // Se for um double
            double tmpDouble = atof(tokens[i + COMAND_POS_IN_LINE]);
            fwrite(&tmpDouble, sizeof(double), 1, database->sourceFile);   
        }
    }
}

/* Faz uma busca binária recursiva dentro do vetor de indexes ordenado */ 
int searchKey(Database *database,int *sortedIndexes, int key, int min, int max) {
    if (max >= min) {
        int mid = min + ((max - min) / 2);
    
        if (database->dataKeys[sortedIndexes[mid]] < key)
            return searchKey(database, sortedIndexes, key, mid + 1, max);
    
        if (database->dataKeys[sortedIndexes[mid]] > key)
            return searchKey(database, sortedIndexes, key, min, mid - 1);

        return sortedIndexes[mid];  

    }
    
    return -1;   // Caso não ache o index
}

void searchIndex(Database *database, int *sortedIndexes, int key) {
    int index = searchKey(database, sortedIndexes, key, 0, database->amountOfData - 1);
    
    if (index == -1)
        return;

    fseek(database->sourceFile, index * database->metadata->offsetSize, SEEK_SET); // Bota o ponteiro na posição certa

    for (int i = 0; i < database->metadata->amountOfFields; ++i) {
            printf("%s: ", database->metadata->fields[i].name); // Printa o nome do campo

            if (database->metadata->fields[i].type == STRING) {
                char *tmpCharPointer = malloc(database->metadata->fields[i].size); //Cria um ponteiro temporário para ler (mesma coisa para os outros tipos)
                fread(tmpCharPointer, sizeof(char), database->metadata->fields[i].size, database->sourceFile);
                printf("%s\n", tmpCharPointer);
                free(tmpCharPointer);
            }

            if (database->metadata->fields[i].type == INTEGER) {
                int tmpInt;
                fread(&tmpInt, sizeof(int), 1, database->sourceFile);
                printf("%i\n", tmpInt);
            }

            if (database->metadata->fields[i].type == FLOAT) {
                float tmpFloat;
                fread(&tmpFloat, sizeof(float), 1, database->sourceFile);
                printf("%.2lf\n", tmpFloat);
            }            

            if (database->metadata->fields[i].type == DOUBLE) {
                double tmpDouble;
                fread(&tmpDouble, sizeof(double), 1, database->sourceFile);
                printf("%.2lf\n", tmpDouble);
            }
    }
    putchar('\n'); // Dá um pulo de linha
}

int main(int argc, char *argv[]) {
    Database *database = initDatabase();
    int amountOfTokens;
    char *line;
    char **tokens;
    int *sortedIndexes;

    int hascreatedIndex = FALSE;
    while (strcmp((line = readLine(stdin)), "exit") != 0) { // é executado enquanto a linha lida for diferente de "exit"
        tokens = tokenfy(line, " ", ", ", &amountOfTokens); // Transforma a linha em tokens e já da free em seguida
        free(line);

        if (strcmp(tokens[COMAND_INDEX_IN_LINE], "insert") == 0) // Se for insert
            insertFieldValues(database, tokens);
        
        else if (hascreatedIndex == FALSE) { // Ou seja, será executado quando o comando não for mais insert e o hascreatedIndx for falso
            sortedIndexes = bubbleSortIndexes(database->dataKeys, database->amountOfData);
            hascreatedIndex = TRUE;
        }
        
        if (strcmp(tokens[COMAND_INDEX_IN_LINE], "search") == 0) 
            searchIndex(database, sortedIndexes, atoi(tokens[1]));

        freeTokens(tokens, amountOfTokens);
    }

    free(line); // Da free na ultima linha pois essa é o "exit"
    free(sortedIndexes);
    freeDatabase(database);
    
    return 0;
}
