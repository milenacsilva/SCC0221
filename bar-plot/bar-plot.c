#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
    int values;
    int amountOfData;
    int value;
    int i;
    
    scanf ("%d %d", &values, &amountOfData);
    /* Cria um vetor que vai armazenar a quantidade de # que
    cada valor deve ter impresso na sua frente */
    int *valueFrequency = (int*) calloc (values, sizeof(int) );

    
    for (i = 0 ; i < amountOfData ; ++i) {
        scanf ("%d", &value) ;
        ++valueFrequency[value];  //Adiciona +1 no index correspondente ao valor escaneado anteriormente 
    }

    /* Imprime a quantidade de # proporcional a frequencia
    de cada valor */
    for (i = 0 ; i < values ; ++i) { //Roda por cada valor
        printf("%d - ", i);
        for (int j = 0 ; j < valueFrequency[i] ; ++j) //Imprime o numero corespondente de #
            putchar('#');
        putchar('\n');
    }

    free(valueFrequency);
    return 0;
}
