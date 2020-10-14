#include <stdio.h>
#include <stdlib.h>

/*Função que lê um input */
char *readInput(int *len){
    char *str = NULL;
    int c;
    int i = 0;
    while( (c = getchar()) != EOF ) {
        str = (char*) realloc ( (void*) str, (i + 1));
        str[i++] = c;
    }
    (str[i-1] == ' ')? (str[i-1] = '\0') : (str[i] = '\0');
    *len = i;
    return str;
}
/* Função responsável pela encriptação, ela 'adiciona' de todos os caracteres-
desde que estes não sejam um epaço ou um tab- uma determinada chave.Assim, transforma-os 
em outros characteres */
void encryption(char *str,int len,int key) {
    int i;
    for(i = 0 ; i < len ; ++i) {
        // Garante que os outros char do ASCII não sejam acessados,mantendo uma cifra 'circular'
        if (i==0 && str[i]>='A' && str[i]<='Z'){ //Executado caso o primeiro char seja maiusculo 
            str[i] += key;
            if(str[i] < 'Z') // Executado caso o encriptação "extravaze" o alfabeto 
                str[i] = ('A' + 1 - (str[i] % 'Z') );
        }
        else if (str[i] >= 'A') {   //Executado para as demais letras, desde que estas não sejam espaços, numeros, ! , . etc
            str[i] += key;
            if(str[i] > 'z')  // Executado caso o encriptação "extravaze" o alfabeto 
                str[i] = ('a' - 1 + (str[i] % 'z') );
        }
    }
}
/* Função responsável pela desencriptação, ela 'subtrai' de todos os caracteres-
desde que estes não sejam um epaço ou um tab- uma determinada chave.Assim, transforma-os 
em outros characteres */
void decryption(char *str,int len,int key) {
    int i;
    for(i = 0 ; i < len ; ++i) {
        // Garante que os outros char do ASCII não sejam acessados,mantendo uma cifra 'circular'
        if (i == 0 && str[i] >= 'A' && str[i] <= 'Z'){ //Executado caso o primeiro char seja maiusculo 
            str[i] -= key;
            if (str[i] < 'A') // Executado caso o desencriptação "extravaze" o alfabeto 
                str[i] = ( 'Z' + 1 - ('A' % str[i]) );
        }
        else if (str[i] >= 'A' ) {    //Executado para as demais letras, desde que estas não sejam espaços, numeros, ! , . etc
            str[i] -= key;
            if (str[i] < 'a') // Executado caso o desencriptação "extravaze" o alfabeto 
                str[i] = ('z' + 1 - ('a' % str[i]) );
        }
    }
}
int main(int argc, char *argv[]) {
    int stringSize;
    char option;
    int key;
    scanf("%c\n", &option); // O '\n' é necessário pra limpar o buffer e o getchar(), da função readInput,funcionar
    scanf("%d\n", &key);
    char *string = readInput(&stringSize); //o &stringSize diz onde a função deve guardar o valor de lenght lido 
    /* Alterna entre encriptar e desencriptar de acordo com a escolha do usuário */
    switch(option) {
        case '1' : 
            encryption(string, stringSize, key); 
            break;
        case '2' : 
            decryption(string, stringSize, key); 
            break;
    }
    printf("%s\n", string);
    free(string);
    return 0;
}

