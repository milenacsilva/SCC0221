#include <stdio.h>
#include <stdlib.h>

/*Função que verifica a cada jogada se existe um vencedor,
procurando os padrãos do jogo da velha*/
int checkWinner(int turn,char **board) {
    int i,j;
    char whosTurn; 

    whosTurn = (turn % 2 == 0)? 'O': 'X'; //Vai definir qual char o loop deve procurar por

    for (i = 0; i < 3 ; ++i){
        for (j = 0 ; j < 3 ; ++j){
            if (i == 0 && 
                board[i][j] == board[i+1][j] && 
                board[i][j] == board[i+2][j] &&
                board[i][j] == whosTurn) { 
                    goto winner; // Verifica as colunas
            }
            else if (j == 0 && 
                    board[i][j] == board[i][j+1] &&
                    board[i][j] == board[i][j+2] &&
                    board[i][j] == whosTurn) { 
                        goto winner; //Verifica as linhas
            }
            else if (i == 0 && j == 0 && 
                    board[i][j] == board[i+1][j+1] &&
                    board[i][j] == board[i+2][j+2] &&
                    board[i][j] == whosTurn) { 
                        goto winner; //Verifica a diagonal principal
            }
            else if (i == 2 && j == 0 &&
                    board[i][j] == board[i-1][j+1] &&
                    board[i][j] == board[i-2][j+2] &&
                    board[i][j] == whosTurn) { 
                        goto winner; //Verifica a diagonal secundária
            }
        }
    }
    return 1; // Se ninguem vencer o loop da main continua
winner: 
    printf("Fim do jogo! O jogador %c venceu!\n", whosTurn);
    return 0; // Quebra o loop da main
}

/*Função que adiciona as novas jogadas no tabuleiro,
colocando as letras X e O de acordo com o numero do
turno ( pares = 'O', impares = 'X') */
void insertPlays(int turn,int pos, char **board) {
    pos -= 1;  //Como as posições do tabuleiro começam a contar do 1 e não do 0, é necessário subtrair um
    if (turn > 0)
        board[pos/3][pos%3] = (turn % 2 == 0)? 'O': 'X'; //Em turnos pares : turnos impares;
}

 /*Função que desenha e completa o meio do tabuleiro de acordo com 
 o que está presente no ponteiro. Por isso, inicialmente este é 
 preenchido com os numeros  */
void drawGame(int turn, int pos, char **board) {
    insertPlays(turn, pos, board); //Primeiro coloca as novas jogadas antes de imprimir o tabuleiro

    for (int i = 0; i  < 3 ; ++i) {
        for (int j = 0 ; j < 3 ; ++j) {
            printf("%c   |",board[i][j]); 
        }
        printf("\n----+----+-----\n");
    }
}

/*Função que aloca um ponteiro duplo, criando o tabuleiro do jogo*/
char **createBoard(void) {  
    int i,j;

    char **board = (char**) malloc(3 * sizeof(char*)); //aloca as linhas
    for(i = 0;  i < 3; ++i) {
        board[i] = (char*) malloc(3 * sizeof(char)) ; //aloca as colunas
    }

    /* Coloca numeros inicialmente em todas as lacunas o tabuleiro */
    for(i = 0; i < 3; ++i) {
        for(j = 0; j < 3 ; ++j) {
            board[i][j]= ((i*3)+j)+'1'; 
        }
    }

    return board;
} 

/* Dá free no ponteiro alocado */
void freeBoard(char** board) {
    for (int i=0;i<3;++i) 
        free(board[i]); 
    free(board);
}
int main(){
    char **board = createBoard(); 
    int option;
    int turn = 0;
    int c;

    /* Imprime o tabuleiro incial do jogo */
    drawGame(turn, 0, board); 
    printf("Jogo da velha!\n");
     
    /* Loopa as jogadas enquanto não há nenhum venceder o
    todas as casas não estiverem ocupadas */
    do { 
        ++turn;

        /* Alterna entre jogadas de O e X  sendo que a primeira é par
        e a segunda é impar */
        printf("Escolha um número para jogar, jogador %c!\n", (turn % 2 == 0)? 'O' : 'X' );
        scanf("%d\n",&option);

        /* Garante q o jogador não escolha uma casa já preenchida */
        while(board[(option-1)/3][(option-1)%3] > '9'){
            printf("Jogada inválida! Escolha outra casa.\n");
            scanf("%d\n",&option);
        }

        drawGame(turn,option,board); //Desenha o tabuleiro

    } while ( (c = checkWinner(turn, board) > 0) && turn < 9); 

    if (turn == 9 && c == 1) //(Caso de empate) o c tem que ser == 1 para garantir que ninguém ganhe apenas na ultima jogada 
        printf("Deu velha!\n"); 

    freeBoard(board);
    
}