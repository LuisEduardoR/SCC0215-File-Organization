// Trabalho 4 - SCC0215 - Organizacao de Arquivos
// Luis Eduardo Rozante de Freitas Pereira 

# include <stdio.h>
# include <stdlib.h>

#include "functionalities.h"

int main(void) {

    // Recebe a funcionalidade a ser executada.
    int func;
    scanf(" %d", &func);

    // Recebe o nome do arquivo csv.
    char *filename = NULL;
    scanf(" %ms", &filename);

    // Guarda o arquivo de entrada.
    FILE* file = NULL;

    // Guarda o retorno das funções.
    int r;

    switch (func) {

    case 1:
        
        file = fopen(filename, "r");
        if(file == NULL)
            r = 1; // Verifica se o arquivo de entrada foi carregado corretamente e da erro caso contrario.
        else {
            r = functionality1_CsvToBin(file); // Executa a função e armazena seu retorno.

            // Fecha o arquivo de entrada ao final da execução.
            fclose(file);  
        }

        break;
    case 2:
        
        file = fopen(filename, "r");
        if(file == NULL)
            r = 1; // Verifica se o arquivo de entrada foi carregado corretamente e da erro caso contrario.
        else {
            r = functionality2_printBin(file); // Executa a função e armazena seu retorno.

            // Fecha o arquivo de entrada ao final da execução.
            fclose(file);  
        }

        break;
    case 3:
        
        file = fopen(filename, "r");
        if(file == NULL)
            r = 1; // Verifica se o arquivo de entrada foi carregado corretamente e da erro caso contrario.
        else {
            r = functionality3_printBinSelect(file); // Executa a função e armazena seu retorno.

            // Fecha o arquivo de entrada ao final da execução.
            fclose(file);  
        }

        break;
    case 4:
        
        file = fopen(filename, "r+");
        if(file == NULL)
            r = 1; // Verifica se o arquivo de entrada foi carregado corretamente e da erro caso contrario.
        else {
            r = functionality4_12_removeRegisters(file, 0); // Executa a função e armazena seu retorno.

            // Fecha o arquivo de entrada ao final da execução.
            fclose(file);  
        }

        break;
    case 5:
        
        file = fopen(filename, "r+");
        if(file == NULL)
            r = 1; // Verifica se o arquivo de entrada foi carregado corretamente e da erro caso contrario.
        else {
            r = functionality5_13_insertRegisters(file, 0); // Executa a função e armazena seu retorno.

            // Fecha o arquivo de entrada ao final da execução.
            fclose(file);  
        }

        break;
    case 6:
        
        file = fopen(filename, "r+");
        if(file == NULL)
            r = 1; // Verifica se o arquivo de entrada foi carregado corretamente e da erro caso contrario.
        else {
            r = functionality6_updateRegisters(file); // Executa a função e armazena seu retorno.

            // Fecha o arquivo de entrada ao final da execução.
            fclose(file);  
        }

        break;
    case 7:
        
        file = fopen(filename, "r");
        if(file == NULL)
            r = 1; // Verifica se o arquivo de entrada foi carregado corretamente e da erro caso contrario.
        else {
            r = functionality7_sortFile(file); // Executa a função e armazena seu retorno.

            // Fecha o arquivo de entrada ao final da execução.
            fclose(file);  
        }

        break;
    case 8:
        
        file = fopen(filename, "r");
        if(file == NULL)
            r = 1; // Verifica se o arquivo de entrada foi carregado corretamente e da erro caso contrario.
        else {
            r = functionality8_fileMerging(file); // Executa a função e armazena seu retorno.

            // Fecha o arquivo de entrada ao final da execução.
            fclose(file);  
        }

        break;
    case 9:
        
        file = fopen(filename, "r");
        if(file == NULL)
            r = 1; // Verifica se o arquivo de entrada foi carregado corretamente e da erro caso contrario.
        else {
            r = functionality9_fileMatching(file); // Executa a função e armazena seu retorno.

            // Fecha o arquivo de entrada ao final da execução.
            fclose(file);  
        }

        break;
    case 10:
        
        file = fopen(filename, "r");
        if(file == NULL)
            r = 1; // Verifica se o arquivo de entrada foi carregado corretamente e da erro caso contrario.
        else {
            r = functionality10_createIndex(file); // Executa a função e armazena seu retorno.

            // Fecha o arquivo de entrada ao final da execução.
            fclose(file);  
        }

        break;
    case 11:
        
        file = fopen(filename, "r");
        if(file == NULL)
            r = 1; // Verifica se o arquivo de entrada foi carregado corretamente e da erro caso contrario.
        else {
            r = functionality11_searchWithIndex(file); // Executa a função e armazena seu retorno.

            // Fecha o arquivo de entrada ao final da execução.
            fclose(file);  
        }

        break;
    case 12:
        
        file = fopen(filename, "r");
        if(file == NULL)
            r = 1; // Verifica se o arquivo de entrada foi carregado corretamente e da erro caso contrario.
        else {
            r = functionality4_12_removeRegisters(file, 1); // Executa a função e armazena seu retorno.

            // Fecha o arquivo de entrada ao final da execução.
            fclose(file);  
        }

        break;
    case 13:
        
        file = fopen(filename, "r");
        if(file == NULL)
            r = 1; // Verifica se o arquivo de entrada foi carregado corretamente e da erro caso contrario.
        else {
            r = functionality5_13_insertRegisters(file, 1); // Executa a função e armazena seu retorno.

            // Fecha o arquivo de entrada ao final da execução.
            fclose(file);  
        }

        break;
    case 14:
        
        file = fopen(filename, "r");
        if(file == NULL)
            r = 1; // Verifica se o arquivo de entrada foi carregado corretamente e da erro caso contrario.
        else {
            r = functionality14_searchCmp(file); // Executa a função e armazena seu retorno.

            // Fecha o arquivo de entrada ao final da execução.
            fclose(file);  
        }

        break;
    
    default:
        break; 

    }

    // Exibe as mensagens de erro, se houverem.
    switch (r) {

    case 1:
        printf("Falha no processamento do arquivo.\n");
        break;

    case 2:
        printf("Registro inexistente.\n");
        break;
    
    default:
        break;
    }

    // Libera a memória usada para guardar o nome do arquivo.
    free(filename);

    return 0;

}