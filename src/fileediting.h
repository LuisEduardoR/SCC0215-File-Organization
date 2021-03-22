// Trabalho 4 - SCC0215 - Organizacao de Arquivos
// Luis Eduardo Rozante de Freitas Pereira 

// Funções usadas para editar o arquivo binário (Trabalho 2).

# ifndef FILEEDITING_H
# define FILEEDITING_H

# ifndef PAGE_SIZE // Tamanho da página de disco.
# define PAGE_SIZE 32000
# endif

# ifndef TRASH_FILLER // Byte a ser usado como lixo.
# define TRASH_FILLER '@'
# endif

#include "servidorregister.h"

// Remove registros com um determinado valor em um campo do arquivo binario.
int removeFromBin(FILE* targetFile, char* fieldName, char* fieldValue, int ignoreStatus);

// Insere um novo registro no arquivo.
int insertOnBin(FILE* targetfile, Register *newReg, int ignoreStatus);

// Atualiza um valor de um registro.
int updateRegisterOnBin(FILE* targetfile, char* fieldName, char* fieldValue, char* updateFieldName, char* updateFieldValue);

#endif