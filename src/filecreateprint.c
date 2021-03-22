// Trabalho 4 - SCC0215 - Organizacao de Arquivos
// Luis Eduardo Rozante de Freitas Pereira 

// Funções usadas para criar e printar o arquivo binário (Trabalho 1).

# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# include "fileediting.h"

# include "servidorregister.h"
# include "servidorheader.h"

# include "servidorlinkedlist.h"
# include "util.h"

// Gera um arquivo binario a partir de um arquivo .csv.
int csvToBin(FILE* inputCsv) {

    // Cria o arquivo de saida.
    FILE* outputBin = NULL;
    outputBin = fopen("arquivoTrab1.bin", "w+");

    // Retorna um erro se o arquivo de saida nao pode ser criado.
    if(outputBin == NULL) return 1;

    // Gera o header do arquivo.
    char tags[] = FIELD_TAGS;
    Header* header = readHeaderFromCsv(inputCsv, tags);
    
    // Gera o cabecalho.
    writeHeaderToBin(header, outputBin);

    // Lista de registros.
    Register **registers = NULL;
    int numRegisters = 0;
    
    // Le os arquivos do .csv para a lista.
    registers = readRegisterFromCsv(registers, &numRegisters, inputCsv);

    // Escreve o arquivo binario.
    writeRegistersToFile(outputBin, header, registers, numRegisters);

    // Libera o header.
    free(header);

    // Libera a lista de registros.
    registers = freeRegisterList(registers, numRegisters);

    // Fecha o arquivo de saida.
    fclose(outputBin);

    return 0;

}

// Carrega os registros de um arquivo binario e printa eles para a tela e marca o numero de paginas de disco acessadas.
int printBin(FILE* inputBin) {

    // Garante que esteja no começo do arquivo.
    fseek(inputBin, 0, SEEK_SET);

    // Le o cabecalho.
    Header* head = readHeaderFromBin(inputBin, 0);

    if(head == NULL || head->status != '1') { // Se o arquivo esta inconsistente ou o cabecalho nao pode ser gerado retorna.
        if(head != NULL)
            free(head);
        return 1;
    } else {

        // Guarda o numerod e registros lidos com sucesso.
        int numRegisters = 0;

        // Le e printa cada registro.
        Register* curRegister = NULL;
        do {

            // Libera a memoria do registro anterior.
            if(curRegister != NULL)
                curRegister = freeRegister(curRegister);

            // Lê o próximo registro.
            curRegister = readNextRegister(inputBin, head);

            // Se a leitura falhar o arquivo acabou.
            if(curRegister == NULL) continue;

            // Se a leitura for um sucesso aumenta o numero de registros lidos.
            numRegisters++;

            // Printa o registro se ele não foi removido.
            if(curRegister->removido == '-')
                printRegister(stdout, curRegister);

        } while (curRegister != NULL);

        if(numRegisters <= 0) { // Se nenhum registro foi lido retorna um valor indicando que os registros nao existem.
            free(head);
            return 2;
        }   

        // Libera o cabecalho.
        free(head);

        return 0;

    }
}

// Carrega os registros de um arquivo binario e printa aqueles que tiverem um determinado valor em um determinado campo  para a tela e marca o 
// numero de paginas de disco acessadas.
int printBinSelective(FILE* inputBin, char *fieldName, char *fieldValue, int *diskPages) {

    // Garante que esteja no começo do arquivo.
    fseek(inputBin, 0, SEEK_SET);

    // Le o cabecalho.
    Header* head = readHeaderFromBin(inputBin, 0);

    if(head == NULL || head->status != '1') { // Se o arquivo esta inconsistente ou o cabecalho nao pode ser gerado retorna.
        if(head != NULL)
            free(head);
        return 1;
    } else {

        // Guarda o numero de registros lidos com sucesso.
        int numRegistersFound = 0;

        // Le e printa cada registro.
        Register* curRegister = NULL;
        do {

            // Libera a memoria do registro anterior.
            if(curRegister != NULL)
                curRegister = freeRegister(curRegister);

            // Lê o próximo registro.
            curRegister = readNextRegister(inputBin, head);

            // Se a leitura falhar o arquivo acabou.
            if(curRegister == NULL) continue;

            // Printa o registro se ele não foi removido.
            if(curRegister->removido == '-') {
                if(cmpRegisterField(curRegister, fieldName, fieldValue) == 0) {
                    printRegisterDetail(stdout, head, curRegister);

                     // Se o valor do campo no registro bater com o valor no campo buscado, aumenta o número de registros..
                    numRegistersFound++;

                    // Como o campo idServidor é único, não ha porque continuar caso ela já tenha sido encontrado.
                    if(strcmp(fieldName, "idServidor") == 0) {
                        curRegister = freeRegister(curRegister);
                        break;
                    }
                }
            }

        } while (curRegister != NULL);

        // Calcula o número de páginas de disco usadas.
        *diskPages = ftell(inputBin) / PAGE_SIZE;
        if(ftell(inputBin) % PAGE_SIZE > 0) (*diskPages)++;

        if(numRegistersFound <= 0) { // Se nenhum registro foi lido retorna um valor indicando que os registros nao existem.
            free(head);
            return 2;
        }   

        // Libera o cabecalho.
        free(head);

        return 0;

    }
}

// Função auxiliar para escrever registros para o binário, sem necessidade de carregar todos para a memória. Porém, o registro anterior 
// é necessário para poder edita-lo caso lixo tenha que ser adicionado na página de disco.
void writeToFile(FILE *file, Header *header, Register *reg, Register *prevReg) {

    // Recalcula o tamanho do registro (necessário para descartar o lixo que pode ter sido inserido nos finais de página de disco).
    reg->tamanhoRegistro = getTamanhoRegistro(reg);
    
    // Verifica se o registro cabe na página de disco, se não cabe adiciona lixo no final da página e atualiza o registro anterior.
    if((ftell(file) % PAGE_SIZE) + (sizeof(char) + sizeof(int) + reg->tamanhoRegistro) > PAGE_SIZE) {

        // Guarda o offset atual.
        long long unsigned curOffset = ftell(file);

        // Calcula a quantidade de lixo a ser inserida.
        int trashAmount = PAGE_SIZE - (ftell(file) % PAGE_SIZE);
        prevReg->tamanhoRegistro += trashAmount;
        
        // Atualiza o tamanho do registro anterior.
        fseek(file, prevReg->byteOffset + sizeof(char), SEEK_SET);
        fwrite(&(prevReg->tamanhoRegistro), sizeof(int), 1, file);
        
        // Retorna para a posição e preenche o resto da página de disco com lixo.
        fseek(file, curOffset, SEEK_SET);
        fillTrash(file, TRASH_FILLER, trashAmount);

    }

    // Escreve o registro no arquivo.
    reg->byteOffset = ftell(file);
    writeRegister(file, header, reg, 0);
    
}