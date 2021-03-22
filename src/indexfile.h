// Trabalho 4 - SCC0215 - Organizacao de Arquivos
// Luis Eduardo Rozante de Freitas Pereira 

# ifndef INDEXFILE_H
# define INDEXFILE_H

# ifndef PAGE_SIZE // Tamanho da página de disco.
# define PAGE_SIZE 32000
# endif

# ifndef TRASH_FILLER // Byte a ser usado como lixo.
# define TRASH_FILLER '@'
# endif

# include "servidorregister.h"

# include <stdio.h>

typedef struct indexheader
{

    char status; // Status do arquivo, usado ao ler o binario do índice.
    int nroRegistros; // Guarda o número de registros no índice.

} IndexHeader;

typedef struct indexregister
{

    char chaveBusca[120]; // Chave de busca do indice. 
    long long unsigned byteOffset; // byteOffset do registro com a chave de busca no arquivo principal.

} IndexRegister;

// Cria um arquivo de índice secundário a partir de um arquivo binário.
int createIndex(FILE *inputFile, FILE* indexFile);

// Escreve o cabeçalho do arquivo de índice para o arquivo (o arquivo será marcado como inconsistente, o que deve ser mudado manualmente depois).
int writeIndexHeader(FILE *indexFile, IndexHeader *indexHeader);

// Escreve uma lista de registros para um arquivo de índice secundário (O número de registros deve estar em indexHead->nroRegistros, 
// o arquivo será marcado para inconsistente, o que deve ser mudado manualmente depois).
int writeRegisterListToIndex(FILE *indexFile, IndexHeader *indexHeader, Register **registerList);

// Le e gera o header de um índice  a partir de um arquivo binario.
IndexHeader *readIndexHeaderFromBin(FILE* inputBin, int ignoreStatus);

// Lê os registros de um índice d ebusca secundário e gera uma lista.
IndexRegister **readIndexFile(FILE *indexFile, IndexHeader *indexHead);

// Busca e printas registros com um determinado valor em um determinado campo usando um arquivo de indíce secundário, retorna o numero de páginas de
// disco usadas para realizar a busca.
int searchWithIndex(FILE *inputFile, FILE *indexFile, char *fieldName, char *fieldValue, int *indexDiskPages, int *diskPages);

// Atualiza um arquivo de índice.
int updateIndexFile(FILE **indexFile, char *indexFileName, IndexHeader *indexHead, IndexRegister **indexList);

// Função de comparação para ordenar registros do índice secundário com chave nomeServidor.
int cmpIndexNomeByte (const void *a, const void *b);

# endif