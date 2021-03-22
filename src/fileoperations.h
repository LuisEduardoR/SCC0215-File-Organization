// Trabalho 4 - SCC0215 - Organizacao de Arquivos
// Luis Eduardo Rozante de Freitas Pereira 

// Funções usadas para realizar operações em um ou mais arquivos (Trabalho 3).

# ifndef FILEOPERATIONS_H
# define FILEOPERATIONS_H

# ifndef PAGE_SIZE // Tamanho da página de disco.
# define PAGE_SIZE 32000
# endif

# ifndef TRASH_FILLER // Byte a ser usado como lixo.
# define TRASH_FILLER '@'
# endif

#include "servidorregister.h"

// Lê um arquivo binário e salva uma nova versão com seus registros ordenados e sem os registros lógicamente removidos.
int createSortedBin(FILE *inputBin, FILE *outputBin);

// Executa o merging entre dois arquivos e salva o resultado para um terceiro.
int mergeFiles(FILE *inFileA, FILE *inFileB, FILE *outputFile);

// Executa o matching entre dois arquivos e salva o resultado para um terceiro.
int matchFiles(FILE *inFileA, FILE *inFileB, FILE *outputFile);

#endif