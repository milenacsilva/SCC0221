#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 4096
#define ERROR_IN_FILE -1

#define AMOUNT_OF_PALETTES 256
#define RESERVED_FIELD_VALUE 0

#define GREY_SCALE_OPTION '2'
#define INVERT_COLOR_OPTION '1'

/* Define alguns tipos de estruturas de dados */
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;

typedef enum _colorScheme { //Para ajudar a legibilidade do código
    BLUE,
    GREEN,
    RED,
} ColorScheme;

typedef struct _header {
	char *signature;
	DWORD fileSize;
	DWORD reserved;
	DWORD displacement;
} Header;

typedef struct _bitmap {
	DWORD bitmapSize;
	DWORD lenght;
	DWORD height;
	WORD amountOfPlanes;
	WORD bitPerPixel;
	DWORD compression;
	DWORD imageSize;
	DWORD pixelMeterHorizontal;
	DWORD pixelMeterVertical;
	DWORD amountOfUsedColors;
	DWORD amountOfImportantColors;
} Bitmap;

typedef struct _colors {
	BYTE blue;
	BYTE green;
	BYTE red;
	BYTE reserved;
} ColorPalette;

typedef struct _imageInfo {
    FILE *inFile;
    FILE *outFile;
	Header *header;
	Bitmap *bitmap;
	ColorPalette *originalPalette;
	ColorPalette *newPalette;
    int **imageMatrix;
    char *outFileName;
    char *option;
} Image;

/* Função que lê uma linha de texto */
char* readLine(FILE *stream) { 
    char *string = NULL;
    int i = 0;

    do {
        if (i % BLOCK_SIZE == 0) {
            string = (char* ) realloc(string,(i / BLOCK_SIZE + 1) * BLOCK_SIZE);
        }
        string[i] = fgetc(stream);
        i++;
    } while (string[i-1] != '\n' && string[i-1] != EOF);
    
    if (i >= 2 && string[i - 2] == '\r') { // Caso tenha o \r no final antes do \n
        string[i - 2] = '\0';
        string = (char* ) realloc(string, i-1);

    }
    else {
        string[i-1] = '\0';
        string = (char* ) realloc(string, i);
    }
    return string;
}

/* Função genérica que lê um uma parte do arquivo de entrada, se necessário passa ele pra isso pra
dentro de um field e escreve no arquivo de saída */
void readAndWriteNumericValue(void *field, Image *image , int size) {
	void *buffer = (void *) calloc(size, sizeof(void));
    fread(buffer, sizeof(void), size, image->inFile);

    if (field != NULL) 
        *((int *)field) = *((int *)buffer); 
        
	fwrite(buffer, sizeof(void), size, image->outFile);
	free(buffer);
}


/* Função que pega os arquivos e tudo referente a eles (tais como erros e a opção de saída) */
int getFiles(Image *image) {
    char *inFileName = readLine(stdin);
    char *str;

    if (!(str = strstr(inFileName, ".bmp"))) { //Se a imagem não ter a extensão bmp
       printf("Arquivo nao eh do formato BMP\n");
       free(inFileName);
       return ERROR_IN_FILE;
    }

	if (!(image->inFile = fopen(inFileName, "rb"))) { //Caso não seja possível abrir o arquivo
        printf("Erro no arquivo\n");
        free(inFileName);
        return ERROR_IN_FILE;
    }
    
    image->option = readLine(stdin);
    image->outFileName = strndup(inFileName, strlen(inFileName) - strlen(str));

    if(image->option[0] == INVERT_COLOR_OPTION) //Cria o nome do arquivo de saída
        strcat(image->outFileName, "Negativo.bmp");
    else 
        strcat(image->outFileName, "PretoBranco.bmp");

	image->outFile = fopen(image->outFileName, "wb");
    
    free(inFileName);   
    return 0;
}

/* Função que inicializa tudo relativo a struct Image */
Image *initImage() {
	Image *image = malloc(sizeof(Image));
    
    if (getFiles(image) == ERROR_IN_FILE) {
        free(image);
        exit(0);
    }

    image->header = malloc(sizeof(Header));
    image->bitmap = malloc(sizeof(Bitmap));
	image->originalPalette = (ColorPalette *) malloc(sizeof(ColorPalette) * AMOUNT_OF_PALETTES);
    image->newPalette = (ColorPalette *) malloc(sizeof(ColorPalette) * AMOUNT_OF_PALETTES);
    image->imageMatrix = NULL;
    return image;
}

/* Função que dá free em tudo relativo a struct Image */
void freeImage(Image *image) {
    fclose(image->inFile);
    fclose(image->outFile);

    if(image->imageMatrix != NULL) {
        for (int i = 0; i < image->bitmap->height; ++i) 
            free(image->imageMatrix[i]);
        free(image->imageMatrix);
    }

    free(image->bitmap);
    free(image->header->signature);
    free(image->header);
    free(image->newPalette);
    free(image->originalPalette);
    free(image->option);
    free(image->outFileName);
    free(image);
}

/* Função para ler o cabeçalho do arquivo */
void getFileHeader(Image *image) {
    //Primeiro lemos a assinatura
	char *buffer =  calloc((sizeof(WORD) + 1), 1);
	fread(buffer, 1, sizeof(WORD), image->inFile); 

    image->header->signature = buffer;
    if (strcmp(image->header->signature, "BM")) { //Caso a assinatura não for "BM"
        printf("Arquivo nao eh do formato BMP\n");
        freeImage(image);
        exit(0);
    }

    fwrite(buffer, 1, sizeof(WORD), image->outFile);

    // Após isso, lemos todos os outros valores numéricos do cabeçalho
    readAndWriteNumericValue(&image->header->fileSize, image, sizeof(DWORD));
    readAndWriteNumericValue(&image->header->reserved, image, sizeof(DWORD));
    readAndWriteNumericValue(&image->header->displacement, image, sizeof(DWORD));

}

/* Função para ler os valores do mapa de bits */
void getBitMap(Image *image) {
    readAndWriteNumericValue(&image->bitmap->bitmapSize, image, sizeof(DWORD));
    readAndWriteNumericValue(&image->bitmap->lenght, image, sizeof(DWORD));
    readAndWriteNumericValue(&image->bitmap->height, image, sizeof(DWORD));
    readAndWriteNumericValue(&image->bitmap->amountOfPlanes, image, sizeof(WORD));
    readAndWriteNumericValue(&image->bitmap->bitPerPixel, image, sizeof(WORD));
    readAndWriteNumericValue(&image->bitmap->compression, image, sizeof(DWORD));
    readAndWriteNumericValue(&image->bitmap->imageSize, image, sizeof(DWORD));
    readAndWriteNumericValue(&image->bitmap->pixelMeterHorizontal, image, sizeof(DWORD));
    readAndWriteNumericValue(&image->bitmap->pixelMeterVertical, image, sizeof(DWORD));
    readAndWriteNumericValue(&image->bitmap->amountOfUsedColors, image, sizeof(DWORD));
    readAndWriteNumericValue(&image->bitmap->amountOfImportantColors, image, sizeof(DWORD));
}

/* Função que lê as paletas de cores do arquivo */
void getOriginalColorPallete(Image *image) {
    for (int i = 0; i < AMOUNT_OF_PALETTES; ++i) {
		void *buffer = (void *) malloc(sizeof(BYTE) * sizeof(ColorPalette));
        
        fread(buffer, sizeof(BYTE), sizeof(ColorPalette), image->inFile);
		
		image->originalPalette[i].blue = ((BYTE *) buffer)[BLUE];
		image->originalPalette[i].green = ((BYTE *) buffer)[GREEN];
		image->originalPalette[i].red = ((BYTE *) buffer)[RED];
		image->originalPalette[i].reserved = RESERVED_FIELD_VALUE;

		free(buffer);
	}
}

/* Função que cria a nova paleta de cores */ 
void getNewPalette(Image *image) {
	switch (image->option[0]) {

	case GREY_SCALE_OPTION: // Caso a opção escolhida seja relativa a foto preto e branco
		for(int i = 0; i < AMOUNT_OF_PALETTES; ++i) {
		    int avarage = (image->originalPalette[i].red + image->originalPalette[i].green + image->originalPalette[i].blue)/3;
		    image->newPalette[i].red = avarage;
		    image->newPalette[i].green = avarage;
		    image->newPalette[i].blue = avarage;
		}
		break;

	case INVERT_COLOR_OPTION:  // Caso a opção escolhida seja relativa a foto negativa
		for(int i = 0; i < AMOUNT_OF_PALETTES; ++i) {
		    image->newPalette[i].red = ~(BYTE)image->originalPalette[i].red;
		    image->newPalette[i].green = ~(BYTE)image->originalPalette[i].green;
		    image->newPalette[i].blue = ~(BYTE)image->originalPalette[i].blue;
		}
		break;
	}
}

/* Função geral que escreve o arquivo de saída */
void writeOutFile(Image *image) {
    getFileHeader(image);
    getBitMap(image);
    getOriginalColorPallete(image);
    getNewPalette(image);
    
	fwrite(&image->newPalette[0], sizeof(void), sizeof(ColorPalette) * AMOUNT_OF_PALETTES, image->outFile);

    image->imageMatrix = (int **) malloc(sizeof(int *) * image->bitmap->height);  //Aloca a matriz da imagem
    for (int i = 0; i < image->bitmap->height; ++i) {
        image->imageMatrix[i] = (int *) calloc(image->bitmap->lenght, sizeof(int));
    }

    int amountOfTrash = ((image->bitmap->lenght % 4) != 0)? 4 - (image->bitmap->lenght % 4): 0; //Calcula a quantidade de lixo em cada linha
    for (int i = image->bitmap->height - 1; i >= 0 ; --i) { //Lê as linhas de tras para frente
    
        for (int j = 0; j < image->bitmap->lenght; ++j) //Caso for um dado importante da matriz que devera ser armazenado
            readAndWriteNumericValue(&image->imageMatrix[i][j], image, sizeof(BYTE));

        for (int j = 0; j < amountOfTrash; ++j) //Caso seja lixo
            readAndWriteNumericValue(NULL, image, sizeof(BYTE));
    }

}

int main() {	
	Image *image = initImage();
    
    writeOutFile(image);

    printf("CABECALHO:\n" // Imprime os dois cabeçalhos do arquivo
    "Iniciais: %s\n"
    "Tamanho do arquivo: %u\n"
    "Reservado: %u\n"
    "Deslocamento, em bytes, para o inicio da area de dados: %u\n"
    "Tamanho em bytes do segundo cabecalho: %u\n"
    "Resolucao: %u x %u\n"
    "Numero de planos: %u\n"
    "Bits por pixel: %u\n"
    "Compressao usada: %u\n"
    "Tamanho imagem: %u\n"
    "Resolucao horizontal: %u pixel por metro\n"
    "Resolucao Vertical: %u pixel por metro\n"
    "Numero de cores usadas: %u\n"
    "Numero de cores importantes: %u\n", image->header->signature, image->header->fileSize, image->header->reserved, image->header->displacement, image->bitmap->bitmapSize, image->bitmap->lenght, image->bitmap->height, image->bitmap->amountOfPlanes, image->bitmap->bitPerPixel, image->bitmap->compression, image->bitmap->imageSize, image->bitmap->pixelMeterHorizontal, image->bitmap->pixelMeterVertical, image->bitmap->amountOfUsedColors, image->bitmap->amountOfImportantColors);

    printf("PALETA ORIGINAL:\n");
    for (int i = 0; i < AMOUNT_OF_PALETTES; ++i) { //Imprime a paleta original
        printf("Paleta[%d]: R:%d G:%d B:%d\n", i, (int) image->originalPalette[i].red, (int) image->originalPalette[i].green, (int) image->originalPalette[i].blue);
    }

    printf("PALETA NOVA:\n");
    for (int i = 0; i < AMOUNT_OF_PALETTES; ++i) { //Imprime a paleta nova
        printf("Paleta[%d]: R:%d G:%d B:%d\n", i, (int) image->newPalette[i].red, (int) image->newPalette[i].green, (int) image->newPalette[i].blue);
    }

    int amountOfTrash = ((image->bitmap->lenght % 4) != 0)? 4 - (image->bitmap->lenght % 4): 0;
    for (int i = 0; i < image->bitmap->height; ++i) { //Imprime a soma das linhas
        long long lineSum = 0;
        for(int j = 0; j < image->bitmap->lenght; ++j)
            lineSum += image->imageMatrix[i][j];

        lineSum -= amountOfTrash;
        printf("Soma linha %d: %lld\n", i, lineSum);

    }
    printf("%s\n", image->outFileName); //Imprime o nome do arquivo
   
    freeImage(image); //Dá free em tudo

    return 0;
}