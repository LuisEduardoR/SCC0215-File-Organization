#ifndef UTIL_H
#define UTIL_H
#include <stdio.h>

// Vai para o início e marca a consistencia do inicio.
void markConsistence(FILE *file, char status);

// Escreve um determinado número de caracteres de lixo para o arquivo.
void fillTrash(FILE *file, char trashFiller, int amount);

// As seguintes funções foram fornecidas pela disciplina para utilização no projeto:

void binarioNaTela1(FILE *ponteiroArquivoBinario);
void binarioNaTela2(char *nomeArquivoBinario);
void trim(char *str);
void scan_quote_string(char *str);

#endif