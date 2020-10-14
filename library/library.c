#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TITLE_SIZE 100

//Define o que um livro deve conter
typedef struct {
    char* title;
    int  returnDate;
} Book; 

//Define uma biblioteca
typedef struct {
    Book* books;
    int totalBooks;
    Book* borrowed;
    int totalBorrowed;
} Library;

/* Limpa o buffer, uma vez que o runcodes coloca /r/n no final da linha */
void flushBuffer() {
    getchar();
    getchar();
}

/* Função que guarda inicialmente todos os livros dentro da biblioteca */
void storeBooks(Library* library) {
    library->books = malloc(sizeof(Book) * (library->totalBooks)); // Aloca um vetor de structs do tamanho da quantidade total de livros na biblioteca
    int returnDate;

    for(int i = 0; i < library->totalBooks; ++i) {
        /* Aloca um espaço pra colocar cada titulo , por mais que seja o mesmo nome, 
        o endereço (que é o que vai ser passado) é diferente */
        char* title = malloc(sizeof(char) * MAX_TITLE_SIZE); 

        scanf("%[^\r]", title);
        flushBuffer();
        scanf("%d", &returnDate);
        flushBuffer();

        library->books[i].title = title;
        library->books[i].returnDate = returnDate;
    } 
}

/* Função que pega o título do livro, tanto pra alugar
quando pra devolver, e printa a respectiva frase do comando */
char* getTitle(char* string) {
    char* title = malloc(sizeof(char ) * MAX_TITLE_SIZE);

    printf ("%s\n", string);
    scanf("%[^\r]", title);
    flushBuffer();

    return title;
}

/* Função que procura um livro e, se estiver dentro das condições do problema,
aluga ele */
void searchBook(Library* library) {
    char* title = getTitle("Digite o livro que voce procura:");
    int bookIndex = -1;

    for (int i = 0; i < library->totalBooks; ++i) { //Procura por todos os livros da biblioteca
        if (strcmp(title, library->books[i].title) == 0) {
            bookIndex = i;
            break;
        }
    }
    
    if (bookIndex < 0) { //Se o livro não existe, encerra a função
        printf("Livro nao encontrado na biblioteca\n");
        return;
    }
    
    if (library->totalBorrowed == 10) { //Se o usuario já não tem 10 livros, encerra a função 
        printf("Voce ja tem 10 livros alugados\n");
        return;
    }
    
    for (int i = 0; i < library->totalBorrowed ; ++i) { //Se o usuario já alugou o livro, encerra a função
        if (strcmp(title, library->borrowed[i].title) == 0) {
            printf("Livro ja alugado\n");
            return;
        }
    }

    /* Caso nenhuma das outras condições de termino seja atingida, aluga o livro */
    library->borrowed[library->totalBorrowed] = library->books[bookIndex];
    library->totalBorrowed += 1;

    printf("%s alugado com sucesso\n", title);

    free(title);
}
/* Função que mostra todos os livros alugados */
void showBorrowed(Library* library) {
    if ( library->totalBorrowed == 0) { //Se o usuário não tem livros alugados, encerra a função
        printf("Voce nao tem livros alugados\n");
        return;
    }

    printf("Voce tem %d livro(s) alugado(s)\n", library->totalBorrowed);

    for (int i = 0; i < library->totalBorrowed; ++i) { //Mostra as informações de cada livro alugado
        printf("Livro nome: %s\n"
               "Devolve-lo daqui %d dias\n", library->borrowed[i].title, library->borrowed[i].returnDate);
    }
}

/* Função que garante que, após retornar um livro, o array seja reordenado
de acordo a ordem do "aluguel" */
void swap(Book *a, Book *b){
    Book t = *b;
    *a = *b;
    *b = t;
}

/* Função que devolve um livro alugado */
void returnBook(Library* library) {
    char* title  = getTitle("Digite o livro que deseja devolver:");

    int bookIndex = -1;
    for (int i = 0; i < library->totalBorrowed; ++i) { //Procura por todos os livros alugados
        if (strcmp(title, library->borrowed[i].title) == 0) {
            bookIndex = i;
            break;
        }
    }

    if (bookIndex < 0) { //Se o usuario não alugou esse livro, encerra a função
        printf("Voce nao possui esse livro\n");
        return;
    }
    /* Se o usuario possui o livro, reordena todo o array de modo a sempre deixar na
    ordem cronologica do aluguel */
    for (int i = bookIndex; i < library->totalBorrowed - 1; ++i) 
        swap(&library->borrowed[i],&library->borrowed[i+1]);
    library->totalBorrowed -= 1;

    printf("Livro %s foi devolvido com sucesso\n", title);
    free(title);
}

/* Inicializa um pointer do tipo Libary e todos os outros parametros dentro desse*/
Library* initLibrary() {
    Library* library = malloc(sizeof(Library));
    int totalBooks;

    scanf("%d", &totalBooks);
    flushBuffer();

    library->totalBooks = totalBooks;
    library->totalBorrowed = 0;
    library->borrowed = malloc(sizeof(Book) * 10);

    storeBooks(library);
    return library;
}

/* Desaloca tudo que foi alocado na heap pra biblioteca, sobretudo,
o espaço pros títulos dos livros */
void endLibrary(Library* libary) {
    int i;

    for (i = 0; i < libary->totalBooks; ++i) {
        free(libary->books[i].title);
    }

    free(libary->books);
    free(libary->borrowed);
    free(libary);
}

int main(int argc, char* argv[]) {
    Library* library = initLibrary();
    char option;

    do { //Executa a interface para o user enquanto este não digitar 4
        scanf("%c", &option);
        flushBuffer();

        switch (option) {
        case '1' :
            searchBook(library);
            break;
        case '2' :
            showBorrowed(library);
            break;
        case '3':
            returnBook(library);
            break;
        }

    } while (option != '4');

    printf("Programa finalizado\n");

    endLibrary(library); 
}