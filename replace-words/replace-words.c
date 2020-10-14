#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define READLINE_BUFFER 4096

/* Função que pega as linhas de uma determinada stream de entrada toda vez que chamada */
char* readLine(FILE* stream) {
    char *str = NULL;
    int i = 0;

    do {
        if (i % READLINE_BUFFER == 0) {
            int j = (i / READLINE_BUFFER + 1) * READLINE_BUFFER;
            str = (char* ) realloc(str, sizeof(char ) * j);
        }
        str[i] = fgetc(stream);
        i++;
    } while (str[i-1] != '\n' && str[i-1] != EOF);

    str = (char* ) realloc(str, sizeof(char ) * i); //Elimina o espaço adicional alocado
    str[i-1] = '\0'; //Torna em uma string

    return str;
}
/*Visto que algumas das frases que vão ser copiadas não tem '\0' no final, já que estão no inicio/meio da oldString, essa função garante que a tempString terá o '\0' pra ser adicionada posteriormente  à newString*/
char* tempCopy(char* toBeCoppied,int size) { 
    char* tempString = (char* ) calloc(size + 1, sizeof(char));
    strncpy(tempString, toBeCoppied, size); //Como o toBeCoppied será o endereço inicial de um array, deve-se garantir que apenas os n elementos correspondentes a size seram copiados;
    return tempString;
}
/* Cria a newString, realocando-a e copiando a oldString ou a newWord pra ela toda vez que chamada */
void makeNewString(char* toBeCoppied, char** newString, int copySize) {
    if (copySize > 0) {
        int lastSize = (*newString != NULL)? strlen(*newString): 0; //necessário para ter o index pra onde será copiado a tempString
        int newSize = lastSize + copySize;

        char* tempString = tempCopy(toBeCoppied, copySize); //Pega a string temporária explicada acima
        *newString = (char *) realloc(*newString, newSize * sizeof(char)); //aloca o tamanho necessário para a nova string a ser copiada

        strcpy(*newString + lastSize, tempString); // copia a tempString para a newString a partit do index lastSize
        free(tempString);
    }

}

/* Função que procura e substituir as palavras em uma nova string ao chamar makeNewString */
char* replaceWords(char* oldString, char* toReplace, char* newWord) {
    char* newString = NULL;
    char* findWord;
    int lastReplaceIndex = 0;
    int copySize = 0;

    while((findWord = strstr(&oldString[lastReplaceIndex], toReplace)) != NULL) { //O loop roda enquanto tiver palavras a serem substituídas
        copySize = strlen(oldString) - strlen(findWord) - lastReplaceIndex; // Dá o tamanho da parte da oldString que não vai ser modificada e deve ser copiada 
        makeNewString(&oldString[lastReplaceIndex], &newString, copySize); //Coloca na newString a parte não modificada da oldString até a 

        lastReplaceIndex += copySize + strlen(toReplace); // Garante que o strstr busque por outras palavras pra substituir partindo do index da última a ter sido substituida
        makeNewString(newWord, &newString, strlen(newWord)); //Coloca a nova palavra no lugar da antiga
    }
    /* Caso tenha algo depois da ultima palavra que foi substituida, ou n tenha nada a ser substituido */
    makeNewString(&oldString[lastReplaceIndex], &newString, strlen(&oldString[lastReplaceIndex])); 
    
    free(findWord);
    return newString;
}

int main(int argc, char* argv[]) {
    char* toReplace = readLine(stdin);
    char* newWord = readLine(stdin);
    char* oldString = readLine(stdin);

    char* newString = replaceWords(oldString, toReplace, newWord);
    printf("%s\n", newString); //Printa a nova palavra

    free(toReplace);
    free(newWord);
    free(oldString);
    free(newString);
    return 0;
}