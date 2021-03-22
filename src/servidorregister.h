// Trabalho 4 - SCC0215 - Organizacao de Arquivos
// Luis Eduardo Rozante de Freitas Pereira 

#ifndef SERVIDOR_REGISTER_H
#define SERVIDOR_REGISTER_H

# include <stdio.h>

# include "servidorheader.h"

// Template para guardar o registro de um servidor na memória.
typedef struct reg
{

    // Informações sobre o registro.
    char removido;
    int tamanhoRegistro;
    long long int encadeamentoLista;

    // Campos de tamanho fixo.
    int idServidor;
    double salarioServidor;
    char telefoneServidor[14];

    // Campos de tamanho variavel.
    char* nomeServidor;
    char* cargoServidor;

    // Variáveis auxiliares, nunca são escritas no arquivo.
    long long int byteOffset;

} Register;

// Cria e inicializa um novo registro.
Register *createRegister(int id, double salario, char* telefone, char* nome, char* cargo);

// Gera um arquivo binario com registros a partir de um arquiv .csv.
Register **readRegisterFromCsv(Register **registers, int *numRegisters, FILE *inputCsv);

// Salva os registros para um arquivo binario.
int writeRegistersToFile(FILE* outputBin, Header *header, Register **registers, int numRegisters);

// Le o proximo registro de um arquivo binario e o retorna.
Register* readNextRegister(FILE* inputBin, Header *header);

// Insere um novo registro em uma determinada posição no arquivo, se recycling é verdadeiro, reutiliza um espaço previamente 
// removido (não rescreve o tamanhoRegistro).
int writeRegister(FILE* targetfile, Header *header, Register *reg, int recycling);

// Remove um registrador do arquivo binario. OBS: essa função não refaz o encadeamneto da lista, este deve ser refeito depois.
int removeRegister(FILE* targetfile, Header *header, Register *reg);

// Imprime o conteudo de um registro para a saida.
void printRegister(FILE* output, Register *reg);

// Imprime o conteudo detalhado de um registro, incluindo o nome dos campos para a saida.
void printRegisterDetail(FILE* output, Header* header, Register *reg);

// Calcula o tamanho do registro (sem os campos removido e tamanhoRegistro).
int getTamanhoRegistro(Register *reg);

// Libera a memoria usada para guardar um registro.
Register *freeRegister(Register *reg);

// Libera uma lista dinamicamente alocada de registros.
Register **freeRegisterList(Register **regs, size_t size);

// Função de comparação para ordenar registros por tamanho.
int cmpRegisterSize (const void *a, const void *b);

// Função de comparação para ordenar registros por idServidor.
int cmpRegisterId (const void *a, const void *b);

// Função de comparação para ordenar registros por nomeServidor usando byteOffset como desempate.
int cmpRegisterNomeByte (const void *a, const void *b);

// Compara um determinado campo do registro do servidor com um determinado valor.
int cmpRegisterField(Register *reg, char* fieldName, char* fieldValue);

#endif