// Trabalho 4 - SCC0215 - Organizacao de Arquivos
// Luis Eduardo Rozante de Freitas Pereira 

#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>

#define FIELD_TAGS "istnc"

typedef struct header
{

    char status; // Status do arquivo, usado ao ler binarios.
    long long int topoLista; // Topo da lista usado ao ler binarios.

    char tagCampo[5]; // Tag de cada campo.
    char desCampo[5][41]; // Descricao de cada campo.

} Header;

// Le e gera um header a partir de um arquivo .csv.
// OBS1: forcedTags sera usado para gerar as tags dos campos, onde o campo 1 recebera o char 1 e assim por diante, caso queira que tags sejam geradas
// automaticamente use NULL.
// OBS2: Essa funcao avancara a leitura do arquivo .csv em uma linha.
Header* readHeaderFromCsv(FILE* inputCsv, char forcedTags[5]);

// Le e gera um header a partir de um arquivo binario.
Header* readHeaderFromBin(FILE* inputBin, int ignoreStatus);

// Escreve o caçalho em um arquivo binário. 
int writeHeaderToBin(Header* header, FILE* outputBin);

#endif