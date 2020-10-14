/*   Prova de Introdução a Ciências da Computação I (ICCI)
      feita por Milena Côrrea da Silva - nº USP 11795401 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#define BLOCK_SIZE 4096
#define FIND_SMALLEST_OPTION 0 
#define FIND_BIGGEST_OPTION  1
#define AMOUNT_OF_PATTERNS 3

/* Define os indexes dos patterns mais a baixo, para ficar mais fácil de ler */
typedef enum {
    P1,
    P2,
    P3,
} Pattern;

/* Struct com todas as respostas necessárias */
typedef struct {
   int amountOfWords;
   char *smallestWord;
   char *biggestWord;
   int amountOfWordsInP1;
   int amountOfPalindromes;
   char *biggestWordInP2;
   char **wordsInP3;
   char *mostSimilarWordToW;
} Answers;

/* Função que le uma linha de um arquivo "stream" */
char *readLine(FILE *stream) {
    char *str = NULL;
    int i = 0;

    if (feof(stream)) //Caso chegue no fim do arquivo / Quebra o loop da getAnswers()
        return NULL;

    do {
        if (i % BLOCK_SIZE == 0) {
            int j = (i / BLOCK_SIZE + 1) * BLOCK_SIZE;
            str = (char* ) realloc(str, sizeof(char ) * j);
        }
        str[i] = fgetc(stream);
        ++i;
    } while (str[i-1] != '\n' && str[i-1] != EOF);

    if (i <= 1) 
        return NULL;   // No caso se ter um \n no final da última linha do input

    str = (char* ) realloc(str, sizeof(char ) * i); //Elimina o espaço adicional alocado
    str[i-1] = '\0'; //Torna em uma string

    return str;
}

/* Função para verificar se uma string é um palindromo de maneira recursiva. Retorna 1 caso seja e 0 caso não seja */
int findPalindromes(char *string, int i, int j) {
    if (i < j)
        return (string[i] == string[j])? findPalindromes(string, i + 1, j - 1): 0;

    return 1;
}

/* Essa é a função "int comparator(const void *, const void *)" utilizada no qsort */
int compareStrings(const void *str1, const void *str2) {
    int cmpResult = strcmp(*(char **)str1, *(char **)str2);
    return cmpResult; 
}

/* Verifica se a palavra é menor ou a maior que a atual menor/maior. Se for, essa passa a ser a maior/menor (dependendo da opção) */
void findSmallestandBiggestWord(char *string, char **currentWord, int option) {
    int stringLenght = strlen(string);
    int currentSize = (*currentWord != NULL)? strlen(*currentWord): 0; 

    if (option == FIND_SMALLEST_OPTION  && currentSize <= stringLenght)  //Se a palavra verificada não é menor(e o pedido é a menor), só encerra a função
        return;
    if (option == FIND_BIGGEST_OPTION && currentSize >= stringLenght) //Se a palavra verificada não é maior(e o pedido é a maior), só encerra a função
        return;

    if(*currentWord != NULL) //Não reescrevermos a heap
        free(*currentWord);
    *currentWord = strdup(string); // É necessário copiar a string pois na getAnswers() eu darei free em cada linha lida
}

/* Verifica se a palavra é mais semelhante que a atual similar. Se for, essa passa a ser considerada a nova similar */
void findSimilarities(char *string, char *wordW, char **currentSimilarWord, int *currentSimilarChars) {
    int similarities = 0;
    char *mainString = wordW;
    char *subString = string;
 
    if (strlen(string) > strlen(wordW)) { // Precisamos saber qual das duas palavras é menor para tratarmos como a substring
        mainString = string; 
        subString = wordW; 
    }
    
    int sizeOfMainString = strlen(mainString);
    int sizeOfSubString = strlen(subString);

    /* Se o tamanho das palavras for diferente, o loop mais externo faz um shift para comparar com a substring. Exemplo:
    mainString = dispoe; subString = ispoe. Primeiro ele vai comparar o dispo com o ispoe e depois o ispoe com o ispoe*/
    for(int i = 0; i <= sizeOfMainString - sizeOfSubString; ++i) { 
        int tmpSimilarities = 0;
        for (int j = 0; j < sizeOfSubString; ++j)
            if (mainString[i + j] == subString[j]) ++tmpSimilarities;

        (tmpSimilarities > similarities)? similarities = tmpSimilarities: 0; 
    }


    if (similarities < *currentSimilarChars || //Caso a atual palavra não seja tão similar quanto a outra comparada
    (similarities == *currentSimilarChars && strlen(*currentSimilarWord) <= strlen(string)))//Se for tão similar quanto, mas maior que a ultima comparada
        return;

    *currentSimilarChars = similarities; 

    if (*currentSimilarWord != NULL) 
        free(*currentSimilarWord);
    *currentSimilarWord = strdup(string); 
}

/* Função principal com relação a struct, ela chamará todas as demais para computar as respostas */
void getAnswers(FILE *file, regex_t *patterns, Answers *answers, int *amountOfWordsInP3) {
    char *wordW = readLine(stdin);
    int currentSimilarChars = 0;
    char *line;

    for (; (line = readLine(file)) ; ++answers->amountOfWords) { // O loop é executado enquanto não chegar no fim do arquivo (ou ter um /n no final dele)
        
        if (answers->amountOfWords == 0) { // Inicializa as menores palavras de acordo com a primeira linha
            answers->smallestWord = strdup(line);
            answers->biggestWord = strdup(line); 
        }

        findSmallestandBiggestWord(line, &answers->smallestWord, FIND_SMALLEST_OPTION); 
        findSmallestandBiggestWord(line, &answers->biggestWord, FIND_BIGGEST_OPTION);

        answers->amountOfPalindromes += findPalindromes(line, 0, strlen(line) - 1);

        findSimilarities(line, wordW, &answers->mostSimilarWordToW, &currentSimilarChars);

        if (!regexec(&patterns[P1], line, 0, NULL, 0))  // Se a palavra for do 1 padrão
            ++answers->amountOfWordsInP1;

        if (!regexec(&patterns[P2], line, 0, NULL, 0)) // Se a palavra for do 2 padrão
            findSmallestandBiggestWord(line, &answers->biggestWordInP2, FIND_BIGGEST_OPTION); 

        if (!regexec(&patterns[P3], line, 0, NULL, 0)) { // Se a palavra for do 3 padrão
            *amountOfWordsInP3 += 1;
            answers->wordsInP3 = (char**) realloc(answers->wordsInP3, sizeof(char*) * (*amountOfWordsInP3));
            answers->wordsInP3[*amountOfWordsInP3 - 1] = strdup(line); // Novamente, é necessário copiar pois vamos dar free na linha
        }   

        free(line); // para não reescrevermos a heap toda vez que a (line = readline()) for chamada
    }

    free(wordW); 
    qsort(answers->wordsInP3, *amountOfWordsInP3, sizeof(char*), compareStrings); //Ordena o array de palavras no padrão P3
}

/* Função que aloca na heap uma struct do tipo Answers e inicializa alguns valores dessa */
Answers *initAnwers() {
    Answers *answers = (Answers*) malloc(sizeof(Answers));
    answers->amountOfWords = 0;
    answers->amountOfWordsInP1 = 0;
    answers->amountOfPalindromes = 0;
    answers->wordsInP3 = NULL;
    answers->biggestWordInP2 = NULL;
    answers->mostSimilarWordToW = NULL;

    return answers;
}

/* Função que dá free nos ponteiros da struct Answers e na própria struct*/
void freeAnswers(Answers *answers) {
    free(answers->smallestWord);
    free(answers->biggestWord);
    free(answers->biggestWordInP2); 
    free(answers->wordsInP3);
    free(answers->mostSimilarWordToW);
    free(answers);
}

int main() {
    /* Primeiro, lemos o nome do arquivo e abrimos ele */
    char *fileName = readLine(stdin);
    FILE *file;
    if (!(file = fopen(fileName, "r")))  
        exit(0); //Caso ele não exista, encerra o programa
    
    /* Depois, criamos um array de regular expressions a partir dos padrões lidos */
    regex_t *patterns = (regex_t*) malloc(sizeof(regex_t) * AMOUNT_OF_PATTERNS);
    for (int i = 0; i < AMOUNT_OF_PATTERNS; ++i) {
        char *expression = readLine(stdin);
        if (regcomp(&patterns[i], expression, REG_EXTENDED|REG_NEWLINE)) // Caso de erro de compilação
            printf("Erro ao compilar");

        free(expression);
    }

    /* Começamos a procurar as respostas para os itens propostos no exercício */
    int amounOfWordsInP3 = 0;
    Answers *answers = initAnwers();
    getAnswers(file, patterns, answers, &amounOfWordsInP3);

    /* Logo em seguida, imprimimos todos as respostas pedidas */
    printf("%d\n%s\n%s\n%d\n%d\n%s\n", answers->amountOfWords, answers->smallestWord, answers->biggestWord, answers->amountOfWordsInP1, answers->amountOfPalindromes, answers->biggestWordInP2);
    for (int i = 0; i < amounOfWordsInP3; ++i) {
        printf("%s\n", answers->wordsInP3[i]);
        free(answers->wordsInP3[i]);
    }
    printf("%s\n", answers->mostSimilarWordToW);
    

    /* Por fim, damos free em tudo que foi alocado (tirando as palavra de P3 uma vez que seria mais simples dar free enquanto imprimiamos (para não repetir o for)*/
    for (int i = 0; i < AMOUNT_OF_PATTERNS; ++i) {
        regfree(&patterns[i]);
    }
    fclose(file);
    free(patterns);
    freeAnswers(answers);
    free(fileName); 

    return 0;
}