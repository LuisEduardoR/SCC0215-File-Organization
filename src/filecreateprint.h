// Trabalho 4 - SCC0215 - Organizacao de Arquivos
// Luis Eduardo Rozante de Freitas Pereira 

// Funções usadas para criar e printar o arquivo binário (Trabalho 1).

# ifndef FILECREATEPRINT_H
# define FILECREATEPRINT_H

# ifndef PAGE_SIZE // Tamanho da página de disco.
# define PAGE_SIZE 32000
# endif

# ifndef TRASH_FILLER // Byte a ser usado como lixo.
# define TRASH_FILLER '@'
# endif

# include <stdlib.h>

# include "servidorregister.h"

// Gera um arquivo binario a partir de um .csv.
int csvToBin(FILE* inputCsv);

// Carrega os registros de um arquivo binario e printa eles para a tela junto com o numero de paginas de disco acessadas.
int printBin(FILE* inputBin);

// Carrega os registros de um arquivo binario e printa aqueles que tiverem um determinado valor em um determinado campo  para a tela e marca o 
// numero de paginas de disco acessadas.
int printBinSelective(FILE* inputBin, char *fieldName, char *fieldValue, int *diskPages);

// Função auxiliar para escrever registros para o binário, sem necessidade de carregar todos para a memória. Porém, o registro anterior 
// é necessário para poder edita-lo caso lixo tenha que ser adicionado na página de disco.
void writeToFile(FILE *file, Header *header, Register *reg, Register *prevReg);

#endif