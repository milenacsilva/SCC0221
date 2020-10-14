
#include <stdio.h>
#include <stdlib.h>


void determinante(int **matriz, int j, int* det, int order) {
    int i = 0;

    /* Calcula o determinante de uma matriz de ordem 3 */
    if(order==3 && j<3) { 
        *det += (matriz[i][j]*matriz[i+1][(j+1)%3]*matriz[i+2][(j+2)%3]);
        *det -= matriz[i][j]*matriz[i+1][(j+2)%3]*matriz[i+2][(j+4)%3]; 
        determinante(matriz,j+1,det,order);  
        /* Função recursiva que roda até quando o j = 3, ou seja, 3 vezes no total de 
        modo a calcular todas as Diagonais Principais e Secundárias conforme a regra 
        de Sarrus */
    }

    /* Calcula o determinante de matriz de ordem 2 */
    if(order==2) { 
        *det += matriz[0][0] * matriz[1][1];
        *det -= matriz[0][1]*matriz[1][0];
    }

    /* Calcula o determinante de matriz de ordem 1 */
    if(order==1) { 
        *det += matriz[0][0];
    }
}

int main(int argc, char *argv[]) {
    int row, col;
    scanf("%d %d", &row, &col);

    /* Condições onde o determinante não será calculado */
    if ( row > 3 || col > 3 ) {
        printf("Entradas invalidas!\n");
        return 0;
    }
    else if( row != col){
        printf("A matriz de entrada nao possui determinante!\n");
        return 0;
    }

    /* Aloca uma matriz quadrada na memória heap */
    int **arr = (int**) malloc(row*sizeof(int*));
    int i,j;
    int num;
    int sum = 0;
    for (i = 0 ; i < row ; ++i) {
        arr[i]= (int *) malloc(col*sizeof(int));
    }

    /* Pega o input dos números da matriz dada */
    for(i = 0 ; i < col ; ++i) {
        for(j = 0 ; j < row ; ++j) {
            scanf("%d", &num);
            arr[i][j] = num;
        }
    }

    determinante(arr,0,&sum,col);
    printf("O determinante da matriz de entrada equivale a %d e tem ordem %d\n",sum,col);

    /* Libera a matriz */
    for (i = 0; i < row ;++i){
        free(arr[i]);
    }
    free(arr);
}

