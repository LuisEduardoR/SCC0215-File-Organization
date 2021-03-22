// Trabalho 3 - SCC0215 - Organizacao de Arquivos
// Luis Eduardo Rozante de Freitas Pereira 

// Funções usadas para realizar operações em um ou mais arquivos (Trabalho 3).

# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# include "fileoperations.h"
# include "filecreateprint.h"

# include "servidorregister.h"
# include "servidorheader.h"

# include "util.h"

// Lê um arquivo binário e salva uma nova versão com seus registros ordenados e sem os registros lógicamente removidos.
int createSortedBin(FILE *inputBin, FILE *outputBin) {

    // Garante que esteja no começo do arquivo.
    fseek(inputBin, 0, SEEK_SET);

    // Le o cabecalho.
    Header* head = readHeaderFromBin(inputBin, 0);

    if(head == NULL || head->status != '1') { // Se o arquivo esta inconsistente ou o cabecalho nao pode ser gerado retorna.
        if(head != NULL)
            free(head);
        return 1;
    } else {

        // Guarda os registros lidos do arquivo.
        Register **registerList = NULL;
        int numRegisters = 0;

        Register* curRegister = NULL;
        int keepRegister; // Usado para marcar que o registro anterior deve ser mantido.
        do {

            // Libera a memoria do registro anterior se ele nao for mantido.
            if(curRegister != NULL && keepRegister == 0)
                curRegister = freeRegister(curRegister);
            keepRegister = 0;

            // Lê o próximo registro.
            curRegister = readNextRegister(inputBin, head);

            // Se a leitura falhar o arquivo acabou.
            if(curRegister == NULL) continue;

            // Adiciona o registro a lista se ele não foi removido.
            if(curRegister->removido == '-') {

                // Recalcula o tamanho do registro (necessário para descartar o lixo que pode ter sido inserido nos finais de página de disco).
                int tamReg = sizeof(long long int) + sizeof(int) + sizeof(double) + ( 14 * sizeof(char));
                if(curRegister->nomeServidor != NULL) {
                    tamReg += strlen(curRegister->nomeServidor) + 6;
                } if(curRegister->cargoServidor != NULL) {
                    tamReg += strlen(curRegister->cargoServidor) + 6;
                }

                curRegister->tamanhoRegistro = tamReg;

                // Insere na lista.
                registerList = realloc(registerList, sizeof(Register*) * (numRegisters + 1));
                registerList[numRegisters] = curRegister;
                numRegisters++;
                
                // Marca que o registra deve ser mantido na memória.
                keepRegister = 1;                
            }
        } while (curRegister != NULL);

        // Ordena os resgistro.
        qsort(registerList, numRegisters, sizeof(Register*), cmpRegisterId);

        if(numRegisters <= 0) { // Se nenhum registro foi lido retorna um valor indicando que os registros nao existem.
            free(head);
            return 2;
        }

        // Escreve o cabeçalho para o arquivo marcando-o como inconsistente.
        head->status = '0';
        head->topoLista = -1;
        writeHeaderToBin(head, outputBin);

        // Escreve os registros em um novo arquivo.
        writeRegistersToFile(outputBin, head, registerList, numRegisters);

        // Marca o arquivo como consistente.
        fseek(outputBin, 0, SEEK_SET);
        head->status = '1';
        markConsistence(outputBin, head->status);

        // Libera a lista de registros.
        registerList = freeRegisterList(registerList, numRegisters);

        // Libera o cabecalho.
        free(head);

        return 0;

    }
}

// Executa o merging entre dois arquivos.
int mergeFiles(FILE *inFileA, FILE *inFileB, FILE *outputFile) {

    // Garante que esteja no começo dos arquivos.
    fseek(inFileA, 0, SEEK_SET);
    fseek(inFileB, 0, SEEK_SET);

    // Le os cabecalhos.
    Header* headA = readHeaderFromBin(inFileA, 0);
    Header* headB = readHeaderFromBin(inFileB, 0);

    // Se um dos arquivos esta inconsistente ou o cabecalho nao pode ser gerado retorna.
    if(headA == NULL || headA->status != '1' || headB == NULL || headB->status != '1') { 
        if(headA != NULL)
            free(headA);
        if(headB != NULL)
            free(headB);
        return 1;
    } else {

        // Copia o header do arquivo A com o status marcado inconsistente e o topoLista -1 para o novo arquivo.
        Header *outHead = NULL;
        outHead = malloc(sizeof(Header));
        memcpy(outHead, headA, sizeof(Header));

        outHead->status = '0';
        outHead->topoLista = -1;

        writeHeaderToBin(outHead, outputFile);

        Register* curRegisterA = NULL; // Guarda o registro lido do arquivo A.
        Register* curRegisterB = NULL; // Guarda o registro lido do arquivo B.

        // Le o primeiro registro do arquivo.
        curRegisterA = readNextRegister(inFileA, headA);
        curRegisterB = readNextRegister(inFileB, headB);

        // Guarda o último registro escrito no arquivo de saída.
        Register *prevRegister = NULL;

        // Faz o merging entre os arquivos.
        do {

            // Remove registros logicamente removidos do caminho.
            while(curRegisterA != NULL && curRegisterA->removido != '-') {
                freeRegister(curRegisterA);
                curRegisterA = readNextRegister(inFileA, headA);
            }
            while(curRegisterB != NULL && curRegisterB->removido != '-') {
                freeRegister(curRegisterB);
                curRegisterB = readNextRegister(inFileB, headB);
            }

            if(curRegisterB == NULL || curRegisterA->idServidor <= curRegisterB->idServidor) { // Escreve o registro do arquivo A no novo arquivo.

                // Se o registro B é igual, descarta-o e le o próximo registro em B.
                if(curRegisterB != NULL && curRegisterA->idServidor == curRegisterB->idServidor) {
                    freeRegister(curRegisterB);
                    curRegisterB = readNextRegister(inFileB, headB);
                }

                // Escreve o registro no arquivo.
                writeToFile(outputFile, outHead, curRegisterA, prevRegister);

                // Libera o registro e guarda esse registro como anterior.
                if(prevRegister != NULL)
                    freeRegister(prevRegister);
                prevRegister = curRegisterA;

                // Le o próximo registro em A.
                curRegisterA = readNextRegister(inFileA, headA);

            } else if(curRegisterA == NULL || curRegisterA->idServidor > curRegisterB->idServidor) { // Escreve o registro do arquivo B no novo arquivo.

                 // Escreve o registro no arquivo.
                writeToFile(outputFile, outHead, curRegisterB, prevRegister);

                // Libera o registro e guarda esse registro como anterior.
                if(prevRegister != NULL)
                    freeRegister(prevRegister);
                prevRegister = curRegisterB;

                // Le o próximo registro em B.
                curRegisterB = readNextRegister(inFileB, headB);

            }

        } while (curRegisterA != NULL || curRegisterB != NULL);

        // Vai a para o inicio do arquivo e marca-o como consistente,
        char status = '1';
        fseek(outputFile, 0, SEEK_SET);
        fwrite(&status, sizeof(char), 1, outputFile);

        // Libera a memória usada pelos registros.
        if(curRegisterA != NULL)
            curRegisterA = freeRegister(curRegisterA);

        if(curRegisterB != NULL)
            curRegisterB = freeRegister(curRegisterB);

        if(prevRegister != NULL)
            prevRegister = freeRegister(prevRegister);

        // Libera os cabecalhos.
        free(headA);
        free(headB);
        free(outHead);

        return 0;
    }
}

// Executa o merging entre dois arquivos.
int matchFiles(FILE *inFileA, FILE *inFileB, FILE *outputFile) {

    // Garante que esteja no começo dos arquivos.
    fseek(inFileA, 0, SEEK_SET);
    fseek(inFileB, 0, SEEK_SET);

    // Le os cabecalhos.
    Header* headA = readHeaderFromBin(inFileA, 0);
    Header* headB = readHeaderFromBin(inFileB, 0);

    // Se um dos arquivos esta inconsistente ou o cabecalho nao pode ser gerado retorna.
    if(headA == NULL || headA->status != '1' || headB == NULL || headB->status != '1') { 
        if(headA != NULL)
            free(headA);
        if(headB != NULL)
            free(headB);
        return 1;
    } else {

        // Copia o header do arquivo A com o status marcado inconsistente e o topoLista -1 para o novo arquivo.
        Header *outHead = NULL;
        outHead = malloc(sizeof(Header));
        memcpy(outHead, headA, sizeof(Header));

        outHead->status = '0';
        outHead->topoLista = -1;

        writeHeaderToBin(outHead, outputFile);

        Register* curRegisterA = NULL; // Guarda o registro lido do arquivo A.
        Register* curRegisterB = NULL; // Guarda o registro lido do arquivo B.

        // Le o primeiro registro do arquivo.
        curRegisterA = readNextRegister(inFileA, headA);
        curRegisterB = readNextRegister(inFileB, headB);

        // Guarda o último registro escrito no arquivo de saída.
        Register *prevRegister = NULL;

        // Faz o matching entre os arquivos.
        do {

            // Remove registros logicamente removidos do caminho ou que já não podem dar match do caminho.
            while(curRegisterA != NULL && (curRegisterA->removido != '-' || curRegisterA->idServidor < curRegisterB->idServidor)) {
                freeRegister(curRegisterA);
                curRegisterA = readNextRegister(inFileA, headA);
            }

            while(curRegisterB != NULL && (curRegisterB->removido != '-' || curRegisterA->idServidor > curRegisterB->idServidor)) {
                freeRegister(curRegisterB);
                curRegisterB = readNextRegister(inFileB, headB);
            }

            // Se um dos arquivos acaba, já não há mais possibilidade de match.
            if(curRegisterA == NULL || curRegisterB == NULL) break;

            // Verifica se há match, se houver escreve o registro A na saída.
            if(curRegisterA->idServidor == curRegisterB->idServidor) {

                // Descarta o registro B e lê o próximo do arquivo.
                freeRegister(curRegisterB);
                curRegisterB = readNextRegister(inFileB, headB);

                // Escreve o registro no arquivo.
                writeToFile(outputFile, outHead, curRegisterA, prevRegister);

                // Libera o registro e guarda esse registro como anterior.
                if(prevRegister != NULL)
                    freeRegister(prevRegister);
                prevRegister = curRegisterA;

                // Lê o próximo registro de A.
                curRegisterA = readNextRegister(inFileA, headA);

            } 
            else
            {
                // Descarta o registro A e lê o próximo do arquivo.
                freeRegister(curRegisterA);
                curRegisterA = readNextRegister(inFileA, headA);

                 // Descarta o registro B e lê o próximo do arquivo.
                freeRegister(curRegisterB);
                curRegisterB = readNextRegister(inFileB, headB);
            }
            

        } while (curRegisterA != NULL || curRegisterB != NULL);

        // Vai a para o inicio do arquivo e marca-o como consistente,
        char status = '1';
        fseek(outputFile, 0, SEEK_SET);
        fwrite(&status, sizeof(char), 1, outputFile);

        // Libera a memória usada pelos registros.
        if(curRegisterA != NULL)
            curRegisterA = freeRegister(curRegisterA);

        if(curRegisterB != NULL)
            curRegisterB = freeRegister(curRegisterB);

        if(prevRegister != NULL)
            prevRegister = freeRegister(prevRegister);

        // Libera os cabecalhos.
        free(headA);
        free(headB);
        free(outHead);

        return 0;

    }
}