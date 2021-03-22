// Trabalho 4 - SCC0215 - Organizacao de Arquivos
// Luis Eduardo Rozante de Freitas Pereira 

# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# include "filecreateprint.h"
# include "fileediting.h"
# include "fileoperations.h"
# include "indexfile.h"

# include "servidorregister.h"
# include "util.h"

// Executa a funcionalidade 1 do Trabalho 1 da disciplina SCC0215 - Organizacao de Arquivos.
// Lê um arquivo .csv e os escreve em um binário de acordo com a especificação.
int functionality1_CsvToBin(FILE *file) {

    int r = csvToBin(file); // Gera o arquivo binario do .csv

    if(r != 0) return r; // Retorna um código de erro.

    printf("%s\n", "arquivoTrab1.bin"); // Printa o nome do arquivo produzido em caso de sucesso.
    return 0;

}

// Executa a funcionalidade 2 do Trabalho 1 da disciplina SCC0215 - Organizacao de Arquivos.
// Lê um arquivo binário e exibe seus registros na tela.
int functionality2_printBin(FILE *file) {

    // Le o arquivo binario e printa os registros. 
    int r = printBin(file);
    
    if(r != 0) return r; // Retorna um código de erro.

    int diskPages = (ftell(file) / PAGE_SIZE) + 1; // Calcula o numero de paginas de disco acessadas.
    printf("Número de páginas de disco acessadas: %d\n", diskPages); // Printa o numero de páginas acessadas em caso de sucesso.
    return 0;
    

}

// Executa a funcionalidade 3 do Trabalho 1 da disciplina SCC0215 - Organizacao de Arquivos.
// Busca registros com um determinado valor em um determinado campo em um arquivo binário e os exibe na tela.
int functionality3_printBinSelect(FILE *file) {

    // Recebe o nome do arquivo binario, o campo a ser usado e seu valor.
    char fieldname[41] = "\0";
    char fieldvalue[257] = "\0";
    scanf(" %40s %256[^\n\r]", fieldname, fieldvalue);


    // Le o arquivo binario e printa os registros.
    int diskPages;
    int r = printBinSelective(file, fieldname, fieldvalue, &diskPages);
    
    if(r != 0) return r; // Retorna um código de erro.


    printf("Número de páginas de disco acessadas: %d\n", diskPages); // Printa o numero de paginas acessadas em caso de sucesso.
    return 0;

}


// Executa as funcionalidades 4 e 12 dos Trabalhos 2 e 4 da disciplina SCC0215 - Organizacao de Arquivos.
// Remove registros com um determinado valor em um determinado campo de um arquivo binário e atualiza a lista encadeada de registros removidos
// para que seu espaço possa ser reaproveitado, também atualiza o índice se useIndex != 0.
int functionality4_12_removeRegisters(FILE *file, int useIndex) {
 
    // Se necessário, recebe o nome do arquivo de índice.
    char indexFileName[257] = "\0";
    if(useIndex)
        scanf(" %256s ", indexFileName);

    // Número de registros a campos a serem buscados e removidos.
    int n;
    scanf(" %d ", &n);

    // Le o nome do arquivo e o valor de n.
    scanf(" %d ", &n);

    // Cria espaco para guardar os campos dos registros a serem removidos.
    char **fieldname = NULL;
    char **fieldvalue = NULL;

    fieldname = malloc(n * sizeof(char*));
    fieldvalue = malloc(n * sizeof(char*));

    // Aloca espaco e recebe o nome e valor dos campos.
    for(int i = 0; i < n; i++) {

        fieldname[i] = malloc(sizeof(char) * 41);
        fieldvalue[i] = malloc(sizeof(char) * 257);

        scanf(" %40s ", fieldname[i]);
        scan_quote_string(fieldvalue[i]);

    }

    // Guarda o arquivo de índice e seus dados.
    FILE *indexFile = NULL;
    IndexHeader *indexHead = NULL;
    IndexRegister **indexList = NULL;
    // Abre o arquivo de índice e carrega seus dados, se necessário.
    if(useIndex) {

        indexFile = fopen(indexFileName, "r");

        indexHead = readIndexHeaderFromBin(indexFile, 0);

        // Se o arquivo de índice está inconsistente ou o cabecalho nao pode ser gerado retorna.
        if(indexHead == NULL || indexHead->status != '1') {

            if(indexHead != NULL)
                free(indexHead);

            // Libera a memoria usada para guardar o nome e os valores dos campos.
            for(int i = 0; i < n; i++) {

                free(fieldname[i]);
                free(fieldvalue[i]);

            }
            free(fieldname);
            free(fieldvalue);

            // Fecha oa rquivo de índice.
            fclose(indexFile);

            return 1;

        }

        indexList = readIndexFile(indexFile, indexHead);

    }

    // Guarda quantos registros foram removidos.
    int removed = 0;
    // Executa a remocao para os n campos.
    for(int i = 0; i < n; i++) {

        int r = removeFromBin(file, fieldname[i], fieldvalue[i], 0);

        if(r == -1) { // Da um erro de falha no processamento do arquivo.

            removed = 0;
            break;

        }

        // Se um registro é removido e o índice está sendo usado, marca o índice como inconsistente.
        if(useIndex && r > 0 && indexHead->status == '1') {

            indexHead->status = '0';
            fseek(indexFile, 0, SEEK_SET);
            fwrite(&(indexHead->status), sizeof(char), 1, indexFile);

        }

        removed += r;
    }

    // Atualiza o índice se necessário.
    if(useIndex) {

        // Percorre o índice na memória e remove as entradas referentes a registros removidos.
        for(int i = 0; i < n; i++) {

            // Verifica para cada campo passado para remoção.
            if(strcmp(fieldname[i], "nomeServidor\0") == 0) {

                // Percorre a lista e remove os registros do campo.
                for(int j = 0; j < indexHead->nroRegistros; j++) {

                    // Se o registro foi removido, retira-o do arquivo de índice.
                    if(indexList[j] != NULL && strcmp(fieldvalue[i], indexList[j]->chaveBusca) == 0) {

                            free(indexList[j]);
                            indexList[j] = NULL;

                    }
                }

            }
            else
            {
                // Código para a atualização com indíces secundários referentes a outros campos seria adicionado aqui.
            }
        }

        // Reconstroi o índice na memória removendo entradas nulas.
        int p = 0;
        for(int k = 0; k < indexHead->nroRegistros; k++) {

            if (indexList[k] != NULL) {
                indexList[p] = indexList[k];
                p++;
            }

        }
    
        // Atualiza o tamanho do índice.
        indexHead->nroRegistros = p;
        indexList = realloc(indexList, sizeof(IndexRegister*) * indexHead->nroRegistros);

        // Atualiza o arquivo de índice.
        updateIndexFile(&indexFile, indexFileName, indexHead, indexList);

    }

    // Caso algum registro tenha sido removido e o índice não esteja sendo usado, exibe o binário apropriado na tela.
    if(removed > 0) {

        if(!useIndex)
            binarioNaTela1(file);
        else {

            // Fecha e reabre o arquivo no modo escrita para printar o arquivo.
            fclose(indexFile);
            indexFile = fopen(indexFileName, "r");
            binarioNaTela1(indexFile);
        }
    } 

    // Libera a memoria usada para guardar o nome e os valores dos campos.
    for(int i = 0; i < n; i++) {

        free(fieldname[i]);
        free(fieldvalue[i]);

    }
    free(fieldname);
    free(fieldvalue);

    // Libera a memória usada para guardar o índice e fecha o arquivo, se necessário.
    if(useIndex) {
        
        for (int i = 0; i < indexHead->nroRegistros; i++) {
            free(indexList[i]);
        }
        free(indexList);
        
        free(indexHead);

        fclose(indexFile);

    }

    // Exibe um erro caso nenhum registro tenha sido removido.
    if(removed == 0) return 1;

    return 0;

}

// Executa as funcionalidades 5 e 13 dos Trabalhos 2 e 4 da disciplina SCC0215 - Organizacao de Arquivos.
// Insere registros em um arquivo binário fazendo uso do reaproveitamento de registros removidos quando possível, também atualiza o índice se useIndex != 0.
int functionality5_13_insertRegisters(FILE *file, int useIndex) {
  
    // Se necessário, recebe o nome do arquivo de índice.
    char indexFileName[257] = "\0";
    if(useIndex)
        scanf(" %256s ", indexFileName);

    // Número de registros a serem inserido.
    int n;
    scanf(" %d ", &n);

    // Cria espaco para guardar os campos dos registros a serem inseridos.
    int *idServidor = malloc(n * sizeof(int));
    double *salarioServidor = malloc(n * sizeof(double));

    char **telefoneServidor = malloc(n * sizeof(char*));

    char **nomeServidor = malloc(n * sizeof(char*));
    char **cargoServidor = malloc(n * sizeof(char*));

    // Le os registros a serem inseridos.
    for(int i = 0; i < n; i++) {            

        // Buffer usado para guardar campos que podem receber NULO. 
        char buffer[257];

        scanf(" %d ", idServidor + i); // Lê o id do servidor a ser inserido.

        // Lê o salario do servidor a ser inserido.
        scan_quote_string(buffer);
        if(strcmp(buffer, "\0") == 0)
            salarioServidor[i] = -1;
        else
            salarioServidor[i] = atof(buffer);

        // Lê o telefone do servidor a ser inserido.
        telefoneServidor[i] = malloc(sizeof(char) * 14); // ALoca espaço para o campo.

        scan_quote_string(buffer);
        if(strcmp(buffer, "\0") == 0) {

            char fillPhone[14] = "\0@@@@@@@@@@@@@";
            for(int j = 0; j < 14; j++)
                telefoneServidor[i][j] = fillPhone[j];

        } else {

            for(int j = 0; j < 14; j++)
                telefoneServidor[i][j] = buffer[j];

        }

        // Lê o nome do servidor a ser inserido.
        scan_quote_string(buffer);
        if(strcmp(buffer, "\0") == 0) {
            nomeServidor[i] = NULL;
        } else {
            nomeServidor[i] = malloc(sizeof(char) * 257);
            strcpy(nomeServidor[i], buffer);
        }

        // Lê o cargo do servidor a ser inserido.
        scan_quote_string(buffer);
        if(strcmp(buffer, "\0") == 0) {
            cargoServidor[i] = NULL;
        } else {
            cargoServidor[i] = malloc(sizeof(char) * 257);
            strcpy(cargoServidor[i], buffer);
        }

    }

    // Guarda o arquivo de índice e seus dados.
    FILE *indexFile = NULL;
    IndexHeader *indexHead = NULL;
    IndexRegister **indexList = NULL;

    // Abre o arquivo de índice e carrega seus dados, se necessário.
    if(useIndex) {

        indexFile = fopen(indexFileName, "r");

        indexHead = readIndexHeaderFromBin(indexFile, 0);

        // Se o arquivo de índice está inconsistente ou o cabecalho nao pode ser gerado retorna.
        if(indexHead == NULL || indexHead->status != '1') {

            if(indexHead != NULL)
                free(indexHead);

            // Libera a memoria usada para guardar o nome e os valores dos campos.
            free(idServidor);
            free(salarioServidor);
            for(int i = 0; i < n; i++) {

                free(telefoneServidor[i]);
                free(nomeServidor[i]);
                free(cargoServidor[i]);

            }
            free(telefoneServidor);
            free(nomeServidor);
            free(cargoServidor);

            // Fecha oa rquivo de índice.
            fclose(indexFile);

            return 1;

        }

        indexList = readIndexFile(indexFile, indexHead);

    }

    // Guarda possíveis mensagens de erro.
    int inserted = 0;
    for(int i = 0; i < n; i++) { // Executa a inserção para os n campos.

        // Cria o novo registro.
        Register *newReg = createRegister(idServidor[i], salarioServidor[i], telefoneServidor[i], nomeServidor[i], cargoServidor[i]);

        // Insere o registro no arquivo.
        int r = insertOnBin(file, newReg, 0);

        // Detecta erros.
        if(r == -1) {

            // Libera o registro da memória.
            freeRegister(newReg);

            inserted = 0;
            break;

        }

        // Se um registro é inserido e o índice está sendo usado, marca o índice como inconsistente.
        if(useIndex && r != -1 && indexHead->status == '1') {

            indexHead->status = '0';
            fseek(indexFile, 0, SEEK_SET);
            fwrite(&(indexHead->status), sizeof(char), 1, indexFile);

        }

        // Insere o novo registro no índice da memória.
        if(useIndex){

            if(newReg->nomeServidor != NULL) { // Se nomeServidor não é nulo, adiciona o nome no índice.

                // Cria espaço no final do índice.
                indexList = realloc(indexList, sizeof(IndexRegister*) * (indexHead->nroRegistros + 1));
                indexList[indexHead->nroRegistros] = malloc(sizeof(IndexRegister));
                
                // OBS: o código atual só funciona com índices secundários que utilizam o nomeServidor como chave de busca.

                // Insere o novo registro no final do índice.
                indexList[indexHead->nroRegistros]->byteOffset = r;    
                strcpy(indexList[indexHead->nroRegistros]->chaveBusca, nomeServidor[i]);
                indexHead->nroRegistros++;

            }

        }

        // Libera o registro da memória.
        freeRegister(newReg);

        inserted++;

    }

        
    // Atualiza o índice se necessário.
    if(useIndex) {

        // Reordena o índice.
        qsort(indexList, indexHead->nroRegistros, sizeof(IndexRegister*), cmpIndexNomeByte);

        // Atualiza o arquivo de índice.
        updateIndexFile(&indexFile, indexFileName, indexHead, indexList);
    
    }

    // Caso algum registro tenha sido inserido, exibe o binário apropriado na tela.
    if(inserted > 0) {

        if(!useIndex)
            binarioNaTela1(file);
        else {
            // Fecha e reabre o arquivo no modo escrita para printar o arquivo.
            fclose(indexFile);
            indexFile = fopen(indexFileName, "r");
            binarioNaTela1(indexFile);
        }

    }

    // Libera a memoria usada para guardar os valores dos campos dos registros e o nome do arquivo.
    free(idServidor);
    free(salarioServidor);
    for(int i = 0; i < n; i++) {

        free(telefoneServidor[i]);
        free(nomeServidor[i]);
        free(cargoServidor[i]);

    }
    free(telefoneServidor);
    free(nomeServidor);
    free(cargoServidor);

    // Libera a memória usada para guardar o índice e fecha o arquivo, se necessário.
    if(useIndex) {
        
        for (int i = 0; i < indexHead->nroRegistros; i++) {
            free(indexList[i]);
        }
        free(indexList);
        
        free(indexHead);

        fclose(indexFile);

    }

    // Exibe um erro caso algum registro não possa ser inserido.
    if(inserted == 0) return 1;

    return 0;

}

// Executa a funcionalidade 6 do Trabalho 2 da disciplina SCC0215 - Organizacao de Arquivos.
// Atualiza registros do arquivo binário, os removendo e reinserindo quando necessário.
int functionality6_updateRegisters(FILE *file) {

    // Número de registros a serem atualizados.
    int n;
    scanf(" %d ", &n);

    // Le o nome do arquivo e o valor de n.
    scanf(" %d ", &n);

    // Cria espaco para guardar os campos dos registros a serem buscados e atualziados.
    char **nomeCampoBusca = malloc(n * sizeof(char*));
    char **valorCampoBusca = malloc(n * sizeof(char*));

    char **nomeCampoAtualiza = malloc(n * sizeof(char*));
    char **valorCampoAtualiza = malloc(n * sizeof(char*));

    // Le os registros a serem inseridos.
    for(int i = 0; i < n; i++) {            

        nomeCampoBusca[i] = malloc(sizeof(char) * 41);
        valorCampoBusca[i] = malloc(sizeof(char) * 257);

        nomeCampoAtualiza[i] = malloc(sizeof(char) * 41);
        valorCampoAtualiza[i] = malloc(sizeof(char) * 257);

        scanf(" %40s ",nomeCampoBusca[i]);
        scan_quote_string(valorCampoBusca[i]);

        scanf(" %40s ",nomeCampoAtualiza[i]);
        scan_quote_string(valorCampoAtualiza[i]);

    }
    
    // Guarda quantos registros foram removidos.
    int updated = 0;
    for(int i = 0; i < n; i++) { // Executa a atualização para os n campos.

        int r = updateRegisterOnBin(file, nomeCampoBusca[i], valorCampoBusca[i], nomeCampoAtualiza[i], valorCampoAtualiza[i]);

        if(r == -1) { // Da um erro de falha no processamento do arquivo.

            updated = 0;
            break;

        }

        updated += r;

    }

    // Caso os registros tenham sido atualizados com sucesso, exibe o binário na tela.
    if(updated > 0) binarioNaTela1(file);
    
    // Libera a memoria usada para guardar os valores dos campos dos registros e o nome do arquivo.
    for(int i = 0; i < n; i++) {

        free(nomeCampoBusca[i]);
        free(valorCampoBusca[i]);
        free(nomeCampoAtualiza[i]);
        free(valorCampoAtualiza[i]);

    }
    free(nomeCampoBusca);
    free(valorCampoBusca);
    free(nomeCampoAtualiza);
    free(valorCampoAtualiza);

    // Exibe um erro caso algum registro não possa ser atualizado.
    if(updated == 0) return 1;

    return 0;
    
}

// Executa a funcionalidade 7 do Trabalho 3 da disciplina SCC0215 - Organizacao de Arquivos.
// Lê um arquivo binário, ordena seus registros e salva um novo arquivo.
int functionality7_sortFile(FILE *inputFile) {

    // Recebe o nome do arquivo de saída.
    char outputFileName[257];
    scanf(" %257s ", outputFileName);

    // Abre o arquivo de saída.
    FILE *outputFile = fopen(outputFileName, "w+");

    // Se o arquivo não pode ser criado retorna um erro.
    if(outputFile == NULL) return 1;

    // Ordena o arquivo binário. 
    int r = createSortedBin(inputFile, outputFile);
    
    if(r != 0) {
        
        // Fecha o arquivo de saída.
        fclose(outputFile);

        return r; // Retorna um código de erro.
    }

    // Exibe o binário na tela.
    binarioNaTela1(outputFile);

    // Fecha o arquivo de saída.
    fclose(outputFile);

    return 0;

}

// Executa a funcionalidade 8 do Trabalho 3 da disciplina SCC0215 - Organizacao de Arquivos.
// Lê dois arquivo binário, e salva um novo executando o merging entre eles.
int functionality8_fileMerging(FILE *inputFile) {

    // Recebe o nome do segundo arquivo de entrada.
    char inputFile2Name[257];
    scanf(" %256s ", inputFile2Name);

    // Recebe o nome do arquivo de saída.
    char outputFileName[257];
    scanf(" %256s ", outputFileName);

    // Abre o segundo arquivo de entrada.
    FILE *inputFile2 = fopen(inputFile2Name, "r");

    // Se o arquivo não pode ser criado retorna um erro.
    if(inputFile2 == NULL) return 1;

    // Abre o arquivo de saída.
    FILE *outputFile = fopen(outputFileName, "w+");

    // Se o arquivo não pode ser criado retorna um erro.
    if(outputFile == NULL) {
        fclose(inputFile2);
        return 1;
    }

    // Executa o merging.
    int r = mergeFiles(inputFile, inputFile2, outputFile);
    
    // Fecha o segundo arquivo de entrada.
    fclose(inputFile2);

    if(r != 0) {
        
        // Fecha o arquivo de saída.
        fclose(outputFile);

        return r; // Retorna um código de erro.
    }

    // Exibe o binário na tela.
    binarioNaTela1(outputFile);

    // Fecha o arquivo de saída.
    fclose(outputFile);

    return 0;

}

// Executa a funcionalidade 9 do Trabalho 3 da disciplina SCC0215 - Organizacao de Arquivos.
// Lê dois arquivo binário, e salva um novo executando o matching entre eles.
int functionality9_fileMatching(FILE *inputFile) {

    // Recebe o nome do segundo arquivo de entrada.
    char inputFile2Name[257];
    scanf(" %256s ", inputFile2Name);

    // Recebe o nome do arquivo de saída.
    char outputFileName[257];
    scanf(" %256s ", outputFileName);

    // Abre o segundo arquivo de entrada.
    FILE *inputFile2 = fopen(inputFile2Name, "r");

    // Se o arquivo não pode ser criado retorna um erro.
    if(inputFile2 == NULL) return 1;

    // Abre o arquivo de saída.
    FILE *outputFile = fopen(outputFileName, "w+");

    // Se o arquivo não pode ser criado retorna um erro.
    if(outputFile == NULL) {
        fclose(inputFile2);
        return 1;
    }

    // Executa o matching.
    int r = matchFiles(inputFile, inputFile2, outputFile);
    
    // Fecha o segundo arquivo de entrada.
    fclose(inputFile2);

    if(r != 0) {
        
        // Fecha o arquivo de saída.
        fclose(outputFile);

        return r; // Retorna um código de erro.
    }

    // Exibe o binário na tela.
    binarioNaTela1(outputFile);

    // Fecha o arquivo de saída.
    fclose(outputFile);

    return 0;

}

// Executa a funcionalidade 10 do Trabalho 4 da disciplina SCC0215 - Organizacao de Arquivos.
// Lê um arquivo binário e gera um arquivo de índice secundário.
int functionality10_createIndex(FILE *inputFile) {

    // Recebe o nome do arquivo de índice a ser gerado.
    char indexFileName[257];
    scanf(" %256s ", indexFileName);

    // Abre o arquivo de índice.
    FILE *indexFile = fopen(indexFileName, "w+");

    // Se o arquivo não pode ser criado retorna um erro.
    if(indexFile == NULL) return 1;

    // Cria o índice..
    int r = createIndex(inputFile, indexFile);

    // Detecta erros e retorna os códigos das mensagens.
    if(r < 0) {
        
        // Fecha o arquivo de índice.
        fclose(indexFile);

        return -r; // Retorna um código de erro.
    }

    // Exibe o binário na tela.
    binarioNaTela1(indexFile);

    // Fecha o arquivo de índice.
    fclose(indexFile);

    return 0;

}

// Executa a funcionalidade 11 do Trabalho 4 da disciplina SCC0215 - Organizacao de Arquivos.
// Lê um arquivo de índice secundário e executa uma busca no arquivo principal.
int functionality11_searchWithIndex(FILE *inputFile) {

    // Recebe o nome do arquivo de índice secundário a ser usado.
    char indexFileName[257];
    scanf(" %256s ", indexFileName);

    // Recebe o nome do campo a ser buscado.
    char fieldName[257];
    scanf(" %256s ", fieldName);

    // Recebe o valor do campo a ser buscado.
    char filedValue[257];
    scanf(" %256[^\n\r\t] ", filedValue);

    // Abre o arquivo de índice.
    FILE *indexFile = fopen(indexFileName, "r");

    // Se o arquivo de índice não pode ser aberto retorna um erro.
    if(indexFile == NULL) return 1;

    // Faz a busca.
    int indexDiskPages, diskPages;
    int r = searchWithIndex(inputFile, indexFile, fieldName, filedValue, &indexDiskPages, &diskPages);

    // Exibe as mensagens de erro, dentro desta função e em seguida retorna 0, usado para permitir que a saída siga o padrão desejado.
    // Detecta erros e retorna os códigos das mensagens.
    if(r != 0) {
        
        // Fecha o arquivo de índice.
        fclose(indexFile);

        return r; // Retorna um código de erro.
    }

    // Printa o número de páginas de disco usadas para ler o indíce e acessar o arquivo de dados.
    printf("Número de páginas de disco para carregar o arquivo de índice: %d\n", indexDiskPages);
    printf("Número de páginas de disco para acessar o arquivo de dados: %d\n", diskPages);  


    fclose(indexFile);

    return 0;

}

// Executa a funcionalidade 14 do Trabalho 4 da disciplina SCC0215 - Organizacao de Arquivos.
// Faz a comparação entre a eficiência da busca com e sem o indíce secundário.
int functionality14_searchCmp(FILE *inputFile) {

    // Recebe o nome do arquivo de índice secundário a ser usado.
    char indexFileName[257];
    scanf(" %256s ", indexFileName);

    // Recebe o nome do campo a ser buscado.
    char fieldName[257];
    scanf(" %256s ", fieldName);

    // Recebe o valor do campo a ser buscado.
    char filedValue[257];
    scanf(" %256[^\n\r\t] ", filedValue);

    // Abre o arquivo de índice.
    FILE *indexFile = fopen(indexFileName, "r");

    // Se o arquivo de índice não pode ser aberto retorna um erro.
    if(indexFile == NULL) return 1;

    // Faz a busca.
    printf("*** Realizando a busca sem o auxílio de índice\n");
    int diskPagesNoIndex;
    int r_no_index = printBinSelective(inputFile, fieldName, filedValue, &diskPagesNoIndex);

    // Exibe as mensagens de erro, dentro desta função e em seguida retorna 0, usado para permitir que a saída siga o padrão desejado.
    switch (r_no_index) {
        case 1:
            printf("Falha no processamento do arquivo.\n");
            break;
        case 2:
            printf("Registro inexistente.\n");
            break;   
        default:
            break;
    }

    printf("Número de páginas de disco acessadas: %d\n", diskPagesNoIndex);

    printf("*** Realizando a busca com o auxílio de um índice secundário fortemente ligado\n");
    int diskPagesLoadIndex, diskPagesFileIndex;
    int r_index = searchWithIndex(inputFile, indexFile, fieldName, filedValue, &diskPagesLoadIndex, &diskPagesFileIndex);

    // Exibe as mensagens de erro, dentro desta função e em seguida retorna 0, usado para permitir que a saída siga o padrão desejado.
    switch (r_index) {
        case 1:
            printf("Falha no processamento do arquivo.\n");
            break;
        case 2:
            printf("Registro inexistente.\n");
            break;   
        default:
            break;
    }

    // Printa o número de páginas de disco usadas para ler o indíce e acessar o arquivo de dados.
    printf("Número de páginas de disco para carregar o arquivo de índice: %d\n", diskPagesLoadIndex);
    printf("Número de páginas de disco para acessar o arquivo de dados: %d\n", diskPagesFileIndex);  

    printf("\nA diferença no número de páginas de disco acessadas: %d\n", diskPagesNoIndex - diskPagesFileIndex);  

    fclose(indexFile);

    return 0;   

}