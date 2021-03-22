// Trabalho 4 - SCC0215 - Organizacao de Arquivos
// Luis Eduardo Rozante de Freitas Pereira 

# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# include "indexfile.h"

# include "servidorheader.h"
# include "servidorregister.h"

# include "util.h"

// Cria um arquivo de índice secundário a partir de um arquivo binário.
int createIndex(FILE *inputFile, FILE* indexFile) {

    if(inputFile == NULL || indexFile == NULL) return -1;

     // Le o cabeçalho do arquivo binário.
    Header* head = readHeaderFromBin(inputFile, 0);

    if(head == NULL || head->status != '1') { // Se o arquivo esta inconsistente ou o cabecalho nao pode ser gerado retorna.
        if(head != NULL)
            free(head);
        return -1;
    } else {
        
        // Cria o cabeçalho do índice secundário na memória.
        IndexHeader *indexHead = malloc(sizeof(IndexHeader));
        indexHead->status = '0';
        indexHead->nroRegistros = 0;

        Register **registerList = NULL;

        // Le e printa cada registro.
        Register* curRegister = NULL;
        int keepRegister = 0;
        
        // Le os registros do arquivo binário para a memória.
        do {

            // Libera a memoria do registro anterior.
            if(curRegister != NULL && keepRegister == 0)
                curRegister = freeRegister(curRegister);
            keepRegister = 0;

            // Lê o próximo registro.
            curRegister = readNextRegister(inputFile, head);

            // Se a leitura falhar o arquivo acabou.
            if(curRegister == NULL) continue;

            // Se o registro não foi removido, adiciona-o a lista do índice.
            if(curRegister->removido == '-' && curRegister->nomeServidor != NULL) {

                // Aloca espaço para o registro e o adiciona na lista de registros.
                registerList = realloc(registerList, sizeof(Register*) * (indexHead->nroRegistros + 1));
                registerList[indexHead->nroRegistros] = curRegister;
                keepRegister = 1;

                indexHead->nroRegistros++;

            }

        } while (curRegister != NULL);

        // Ordena os registros.
        qsort(registerList, indexHead->nroRegistros, sizeof(Register*), cmpRegisterNomeByte);

        // Escreve o cabeçalho para o arquivo de índice secundário.
        writeIndexHeader(indexFile, indexHead);

        // Escreve os registros para o arquivo de índice secundário.
        writeRegisterListToIndex(indexFile, indexHead, registerList);

        // Marca o arquivo como consistente.
        indexHead->status = '1';
        markConsistence(indexFile, indexHead->status);

        // Libera a memória usada para guardar os registros.
        for(int i = 0; i < indexHead->nroRegistros; i++)
            freeRegister(registerList[i]);
        free(registerList);

        // Libera a memória dos cabeçalhos.
        free(indexHead);
        free(head);

        return 0;
    }

}

// Escreve o cabeçalho do arquivo de índice para o arquivo (o arquivo será marcado como inconsistente, o que deve ser mudado manualmente depois).
int writeIndexHeader(FILE *indexFile, IndexHeader *indexHeader) {

    if(indexFile == NULL || indexHeader == NULL) return 1;

    // Garante que esteja no início do arquivo.
    fseek(indexFile, 0, SEEK_SET);

    // Marca o arquivo como inconsistente.
    indexHeader->status = '0';
    markConsistence(indexFile, indexHeader->status);

    // Escreve o número de registros no arquivo.
    fwrite(&(indexHeader->nroRegistros), sizeof(int), 1, indexFile);

    // Preenche o restante da página de disco com lixo.
    fillTrash(indexFile, TRASH_FILLER, PAGE_SIZE - (ftell(indexFile) % PAGE_SIZE));

    return 0;

}

// Escreve uma lista de registros para um arquivo de índice secundário (O número de registros deve estar em indexHead->nroRegistros, 
// o arquivo será marcado para inconsistente, o que deve ser mudado manualmente depois).
int writeRegisterListToIndex(FILE *indexFile, IndexHeader *indexHeader, Register **registerList) {

    if(indexFile == NULL || indexHeader == NULL) return 1;

    // Escreve os registros para o arquivo de índice secundário, como eles têm tamanho fixo e cabe um número exato por página de disco, não é
    // necessário verificar se o registro cabe na página atual.
    for(int i = 0; i < indexHeader->nroRegistros; i++) {

        // Obtém o tamanho do nome do servidor incluindo o \0 e o escreve no arquivo;
        int tamNome = sizeof(char) * (strlen(registerList[i]->nomeServidor) + 1);
        fwrite(registerList[i]->nomeServidor, sizeof(char), tamNome, indexFile);

        // Preenche o restante do campo de tamanho fixo com lixo.
        fillTrash(indexFile, TRASH_FILLER, 120 - tamNome);

        // Escreve o byteOffset no arquivo.
        fwrite(&(registerList[i]->byteOffset), sizeof(long long unsigned), 1, indexFile);

    }

    return 0;

}

// Le e gera o header de um índice  a partir de um arquivo binario.
IndexHeader* readIndexHeaderFromBin(FILE* inputBin, int ignoreStatus) {

    // Aloca espaco para o header.
    IndexHeader* head = malloc(sizeof(IndexHeader));
    if(head == NULL) // Retorna um erro se a alocacao falhar.
        return NULL;

    // Inicializa com valores padrão.
    head->status = '0';
    head->nroRegistros = 0;

    // Garante que esteja no ínicio do arquivo.
    fseek(inputBin, 0, SEEK_SET);

    // Le o status do arquivo.
    fread(&(head->status), sizeof(char), 1, inputBin);

    if(head->status != '1' && ignoreStatus == 0) { // Retorna um erro caso o arquivo nao esteja marcado como consistente.
        free(head);
        return NULL;
    }

    // Le o encadeamento da lista.
    fread(&(head->nroRegistros), sizeof(int), 1, inputBin);

    // Seta o arquivo para o inicio da segunda pagina de disco.
    fseek(inputBin, PAGE_SIZE, SEEK_SET);

    // Retorna o header lido.
    return head;

}

// Lê os registros de um índice d ebusca secundário e gera uma lista.
IndexRegister **readIndexFile(FILE *indexFile, IndexHeader *indexHead) {

    if(indexFile == NULL || indexHead == NULL || indexHead->status != '1') return NULL;

    // Aloca espaço para a lista.
    IndexRegister **indexList = malloc(sizeof(IndexRegister) * indexHead->nroRegistros);

    // Lê os registros do índice.
    for(int i = 0; i < indexHead->nroRegistros; i++) {

        // Aloca espaço para or egistro.
        indexList[i]= malloc(sizeof(IndexRegister));

        // Lê o conteúdo.
        fread(indexList[i]->chaveBusca, sizeof(char), 120, indexFile);
        fread(&(indexList[i]->byteOffset), sizeof(long long unsigned), 1, indexFile);

    }

    // Retorna a lista de registros.
    return indexList;

}

// Realiza uma busca binária por registros com um determinado valor em um determinado campo usando um índice secundário do campo nomeServidor e printa 
// suas informações, retorna o número de páginas de disco acessadas para achar o registro ou -1 se nenhum registro válido for encontrado. 
int printSelectiveBinSearchNomeServidor(FILE *inputFile, IndexRegister **indexList, Header *head, IndexHeader *indexHead, char *fieldValue, int *diskPages) {

    // Coloca o arquivo binário na primeira página de disco após o cabeçalho.
    fseek(inputFile, PAGE_SIZE, SEEK_SET);

    // Minímo e máximo da busca binária.
    int max = indexHead->nroRegistros - 1;
    int min = 0;

    // Realiza a busca binária.
    int cur;
    int found = 0;
    while (found == 0 && min <= max) {

        // Calcula a posição da busca binária.
        cur = (max + min) / 2;

        // Faz a comparação.
        int cmp = strcmp(fieldValue, indexList[cur]->chaveBusca);

        // Verifica se a chave foi encontrada.
        if(cmp == 0) found = 1;
        else if (cmp > 0) min = cur + 1;
        else max = cur - 1;     

    }

    if(!found)
        return 2; // Se a chave não foi encontrada retorna um código de erro.

    // Se achave foi encontrada:

    // Retorna no indíce até encontrar o primeiro match.
    while (cur - 1 > 0 && strcmp(fieldValue, indexList[cur - 1]->chaveBusca) == 0) cur --;
    
    
    // Percorre o indíce verificando e printando todos os registros válidos no arquivo de dados.
    int prevDiskPage = 0; // Guarda a última página de disco acessada no arquivo de dados, usado para calulcar o total de páginas acessadas.
    while (cur < indexHead->nroRegistros && strcmp(fieldValue, indexList[cur]->chaveBusca) == 0)
    {
        // Vai para a posição no arquivo de dados.
        fseek(inputFile, indexList[cur]->byteOffset, SEEK_SET);

        int curDiskPage = ftell(inputFile) / PAGE_SIZE; // Pega em que página de disco o arquivo está.
        if(curDiskPage != prevDiskPage) (*diskPages)++; // Se a página atual é diferente da página anterior, então uma nova página foi acessada.
        prevDiskPage = curDiskPage; // Atualiza a página anterior acesssada.

        // Lê o registro do arquivo.
        Register *curRegister = readNextRegister(inputFile, head);
        printRegisterDetail(stdout, head, curRegister); // Printa o registro.
        freeRegister(curRegister);// Libera a memória no registro.

        cur++; // Checa o próximo registro ni indíce.

    }

    return 0;    

}

// Busca e printas registros com um determinado valor em um determinado campo usando um arquivo de indíce secundário, retorna o numero de páginas de
// disco usadas para realizar a busca.
int searchWithIndex(FILE *inputFile, FILE *indexFile, char *fieldName, char *fieldValue, int *indexDiskPages, int *diskPages) {

    if(inputFile == NULL || indexFile == NULL) return 1;

    // Le o cabeçalho dos arquivos.
    Header* head = readHeaderFromBin(inputFile, 0);
    (*diskPages) = 1; // Conta que a página de cabeçalho foi lida.
    IndexHeader* indexHead = readIndexHeaderFromBin(indexFile, 0);

    if(head == NULL || head->status != '1' || indexHead == NULL || indexHead-> status != '1') { // Se um dos arquivos esta inconsistente ou um
                                                                                                // cabecalho nao pode ser gerado retorna.
        if(head != NULL)
            free(head);
        if(head != NULL)
            free(indexHead);

        return 1;
    } else {

        // Carrega os registros do índice.
        IndexRegister **indexList = readIndexFile(indexFile, indexHead);

        // Calcula quantas páginas de disco foram usadas para ler o índice.
        *indexDiskPages = ftell(indexFile) / PAGE_SIZE;
        if(ftell(indexFile) % PAGE_SIZE != 0)
            (*indexDiskPages)++;

        // Guarda o número de páginas de disco acessadas no arquivo de dados.
        int r;
        if(strcmp("nomeServidor\0", fieldName) == 0)
            r = printSelectiveBinSearchNomeServidor(inputFile, indexList, head, indexHead, fieldValue, diskPages);
        else
        {
            // Código para realizar a busca binária com indíces secundários referentes a outros campos seria adicionado aqui.
        }

        // Se o registro não foi encontrado retorna uma mensagem de erro.
        if(r != 0) {

            // Libera a memória dos registros.
            for(int i = 0; i < indexHead->nroRegistros; i++)
                free(indexList[i]);
            free(indexList);

            // Libera os cabeçalhos.
            free(head);
            free(indexHead);

            return r;

        }

        // Libera a memória dos registros.
        for(int i = 0; i < indexHead->nroRegistros; i++)
            free(indexList[i]);
        free(indexList);

        // Libera os cabeçalhos.
        free(head);
        free(indexHead);

    }

    return 0;

}

// Atualiza um arquivo de índice.
int updateIndexFile(FILE **indexFile, char *indexFileName, IndexHeader *indexHead, IndexRegister **indexList) {

    // Fecha o arquivo e reabre no modo escrita.
    fclose(*indexFile);
    (*indexFile) = fopen(indexFileName, "wb");

    // Atualiza o arquivo.
    writeIndexHeader((*indexFile), indexHead);
    // Escreve os registros para o arquivo de índice secundário, como eles têm tamanho fixo e cabe um número exato por página de disco, não é
    // necessário verificar se o registro cabe na página atual.
    for(int i = 0; i < indexHead->nroRegistros; i++) {

        // Obtém o tamanho do nome do servidor incluindo o \0 e o escreve no arquivo;
        int tamNome = sizeof(char) * (strlen(indexList[i]->chaveBusca) + 1);
        fwrite(indexList[i]->chaveBusca, sizeof(char), tamNome, (*indexFile));

        // Preenche o restante do campo de tamanho fixo com lixo.
        fillTrash(*indexFile, TRASH_FILLER, 120 - tamNome);

        // Escreve o byteOffset no arquivo.
        fwrite(&(indexList[i]->byteOffset), sizeof(long long unsigned), 1, (*indexFile));
    }

    // Marca o arquivo como consistente.
    indexHead->status = '1';
    markConsistence((*indexFile), indexHead->status);

    // Retorna para o começo do arquivo.
    fseek(*indexFile, 0, SEEK_SET);

    return 0;

}


// Função de comparação para ordenar registros do índice secundário com chave nomeServidor.
int cmpIndexNomeByte (const void *a, const void *b) {

    IndexRegister *ptr1 = *(IndexRegister * const *)a;
    IndexRegister *ptr2 = *(IndexRegister * const *)b;

    int diff = strcmp(ptr1->chaveBusca, ptr2->chaveBusca);
    if(diff == 0) {
        return (ptr1->byteOffset - ptr2->byteOffset);
    }
    return diff;

}