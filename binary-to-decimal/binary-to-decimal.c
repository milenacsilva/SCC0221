#include <stdio.h>
#include <math.h>
#define SIZEOFINT 32 // tamanho do int em bits em vez de bytes

/*Converte um input binario para decimal*/
int main() {
    char binary[SIZEOFINT+1];
    int i = 0;
    char c;
    long long int decimal_num = 0;
    while((c=getchar()) != EOF) {  // O loop vai parar quando o getchar() chegar no END_OF_FILE
        binary[i++] = (c == '1')? 1 : 0;  //Se o char lido for 1 então o binary[i] terá armazenado o int 1; Se não, terá armazenado o 0
    }
    --i;// é necessário,visto que, o loop adicionou +1 ao i antes de verificar a condição de termino
    for(int j=0 ; j<=i ; ++j) { //roda apenas sobre os elementos alterados do array
        decimal_num += (pow( 2 , i-j ) * binary[j]);  //eleva o 2 ao inverso do seu index (enquanto binário) correspondente e multiplica pelo dígito binário
    }
    printf("%lld\n", decimal_num);
    return 0;
}