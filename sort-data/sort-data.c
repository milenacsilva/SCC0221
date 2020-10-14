#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define READLINE_BUFFER 4096

#define SORTING_OPTION(option,i,struct) ( (option == '1')? strcmp(struct[i]->string, struct[i+1]->string) > 0 : struct[i]->integer > struct[i+1]->integer ) // Macro que alterna o tipo do sorting realizado dentro do bubbleSort

typedef struct {
    long long integer;
    char *string;
} Data;

/* Função que pega as linhas de uma determinada stream de entrada toda vez que chamada */
char* readLine(FILE* stream) {
    char *line = NULL;
    int i = 0;

    if (feof(stream)) //Quebra o loop dentro do storeMetadata caso chegue no fim do arquivo ao retornal NULL
        return NULL;

    do {
        if (i % READLINE_BUFFER == 0) {
            int j = (i / READLINE_BUFFER + 1) * READLINE_BUFFER;
            line = (char* ) realloc(line, sizeof(char ) * j);
        }
        line[i] = fgetc(stream);
        ++i;
    } while (line[i-1] != '\n' && line[i-1] != EOF);
    line = (char* ) realloc(line, sizeof(char ) * i); //Elimina o espaço adicional alocado
    line[i-1] = '\0'; //Torna em uma string
    
    return line;
}

/* Função que retorna o index de onde um tipo de dado termina- nesse caso, onde a string termina-
e onde o próximo- no caso o número inteiro- começa */
int getSeparateDataIndex(char *line) {
    int lineLenght = strlen(line);
    for (int i = 0; i < lineLenght; ++i) {
        if (isdigit(line[i]) != 0 || ( line[i] == '-' && isdigit(line[i+1]) != 0) ) {
            return i;
        } 
    }
    return 0; // Caso não tenha nenhum número na frente
}

/*Função que aloca o ponteiro duplo de structs */
void allocData(Data ***data, int amountOfData) { 
    *data = (Data** ) realloc(*data, sizeof(Data* ) * amountOfData); 
    (*data)[amountOfData - 1] = (Data* ) malloc(sizeof(Data)); 
}

/* Função que lê os dados e guarda-os dentro de um array duplo de structs */
Data** storeData(int *amountOfData) {
    Data **data = NULL;
    char *line;
    int i;

    for (i = 0; (line = readLine(stdin)) != NULL; ++i) { //Roda enquanto tiver dados a serem lidos, ou seja, enquanto a readLine não chegar em feof
        allocData(&data, i + 1); 
        int separateDataIndex = getSeparateDataIndex(line); //Acha o index que separa um tipo de metadado do outro

        data[i]->string = strndup(line, separateDataIndex - 1);
        data[i]->integer = atoll(&line[separateDataIndex]);

        free(line); //Dá free nas line que acabou de ser lida, visto que não é mais necessária (evita de reescrevermos a heap)
    }
    
    *amountOfData = i; //Passa por referencia a quantidade de dados a partir da quantidade de linhas lidas
    return data; 
}

/* Parte do bubbleSort que troca a posição dos ponteiros simples tipo Data */
void swap(Data *a, Data *b) {
    Data tmp = *a; 
    *a = *b;
    *b = tmp;
}

/* bubbleSort que ordenará o registro de acordo com o modo escolhido */
void sortData(Data **data, int amountOfData, char option) {
    for (int j = 1; j < amountOfData; j++) {
        for (int i = 0; i < amountOfData - 1; i++) {
            if ( SORTING_OPTION(option,i,data)) {
                swap(&*data[i], &*data[i+1]);
            }
         }
    }
}

int main(int argc, char *argv[]) {
    int amountOfData;
    char sortOption;

    scanf("%c\n", &sortOption); //Pega a primeira linha, correspondente ao tipo de ordenamento dos registros
    Data **data = storeData(&amountOfData); //Coloca todos os registros dentro de um array duplo

    sortData(data, amountOfData, sortOption); //Ordena os registros

    for(int i = 0; i < amountOfData; ++i) { // Printa os registros já ordenados ao mesmo tempo que dá free no que já foi printado
        printf("%lld\t%s\n", data[i]->integer, data[i]->string);
        free(data[i]->string);
        free(data[i]);
    }

    free(data); 
    return 0;
}