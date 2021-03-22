// Trabalho 4 - SCC0215 - Organizacao de Arquivos
// Luis Eduardo Rozante de Freitas Pereira 

// Funções usadas para editar o arquivo binário (Trabalho 2).

# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# include "fileediting.h"

# include "servidorregister.h"
# include "servidorheader.h"

# include "servidorlinkedlist.h"
# include "util.h"

// Remove registros com um determinado valor em um campo do arquivo binario.
int removeFromBin(FILE* inputBin, char *fieldName, char *fieldValue, int ignoreStatus) {

    // Garante que esteja no começo do arquivo.
    fseek(inputBin, 0, SEEK_SET);

    // Le o cabecalho.
    Header* head = readHeaderFromBin(inputBin, ignoreStatus);

    // Guarda os registros que foram removidos agora.
    Register **registersRemoved = NULL;
    int numRegistersRemoved = 0;

    if(head == NULL || (head->status != '1' && ignoreStatus == 0)) { // Se o arquivo esta inconsistente ou o cabecalho nao pode ser gerado retorna.
        if(head != NULL)
            free(head);
        return -1;
    } else {

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

            // Se o registro não foi removido, verifica se eé preciso removê-lo.
            if(curRegister->removido == '-') {

                if(cmpRegisterField(curRegister, fieldName, fieldValue) == 0) { // Verifica se esse registro é um alvo da remoção.

                    // Se este eh o primeiro registro a ser removido marca que o arquivo esta inconsistente até o fim da operação.
                    if(numRegistersRemoved == 0) {

                        // Guarda a posição atual para retornar.
                        long long int curOffset = ftell(inputBin);

                        // Vai a para o inicio do arquivo e marca-o como inconsistente.
                        head->status = '0';
                        markConsistence(inputBin, head->status);

                        // Retorna para a posição.
                        fseek(inputBin, curOffset, SEEK_SET);

                    }

                    // Remove o registro.
                    removeRegister(inputBin, head, curRegister);

                    // Adiciona o registro a lista de removidos.
                    registersRemoved = realloc(registersRemoved, (numRegistersRemoved + 1) * (sizeof(Register*)));
                    registersRemoved[numRegistersRemoved] = curRegister;
                    numRegistersRemoved++;  

                    // Marca que o registra deve ser mantido na memória.
                    keepRegister = 1;                

                    // Se o campo a ser removido era do tipo idServidor, não é necessário continuar.
                    if(strcmp(fieldName, "idServidor\0") == 0) break;                     

                }
            }

        } while (curRegister != NULL);

        // Se necessário atualiza o encademaneto da lista.
        if(numRegistersRemoved > 0) {

            // Cria uma nova lista encadeada.
            ListaEnc *list = createLinkedList();

            // Verifica se a lista encadeada do registro não está vazia.
            if(head->topoLista != -1) {

                // Posiciona no primeiro elemento da lista encadeada.
                fseek(inputBin, head->topoLista, SEEK_SET);

                // Lê os registros da lista encadeada.
                Register *reg = NULL;
                do {

                    // Lê o próximo registro do encadeamento.
                    reg = readNextRegister(inputBin, head);

                    if(reg == NULL) break;  // Verifica se a lista acabou.

                    // Insere o registro na lista.
                    insertLinkedListBack(list, reg);

                    if(reg->encadeamentoLista == -1) break;  // Verifica se a lista acabou.

                    // Posiciona no proximo elemento da lista encadeada.
                    fseek(inputBin, reg->encadeamentoLista, SEEK_SET);

                } while(reg != NULL && reg->encadeamentoLista != -1);

            }

            // Insere os registros removidos na lista encadeada.
            for(int i = 0; i < numRegistersRemoved; i++)
                insertLinkedListFE(list, registersRemoved[i]);

            Register *cur = NULL;
            Register *prev = NULL;

            if(getSizeLinkedList(list)) {

                // Pega o primeiro elemento da lista encadeada e atualiza topoLista no cabecalho.
                cur = getLinkedList(list, 0);

                // Posiciona o aquivo no campo topoLista do cabecalho e o atualiza.
                fseek(inputBin, sizeof(char), SEEK_SET);
                fwrite(&(cur->byteOffset), sizeof(long long int), 1, inputBin);

                for(int i = 1; i < getSizeLinkedList(list); i++) {

                    prev = cur;
                    cur = getLinkedList(list, i);

                    // Atualiza o encadeamento da lista.
                    fseek(inputBin, prev->byteOffset + sizeof(char) + sizeof(int), SEEK_SET);
                    fwrite(&(cur->byteOffset), sizeof(long long int), 1, inputBin);

                }

                // Atualiza o encadeamento do ultimo registro na lista.
                long long int endEnc = -1;
                fseek(inputBin, cur->byteOffset + sizeof(char) + sizeof(int), SEEK_SET);
                fwrite(&endEnc, sizeof(long long int), 1, inputBin);

            }

            // Libera a memória utilizada. OBS: como os ponteiros dos registros em removedRegisters foram copiados para dentro da lista encadeada, não se deve
            // libera-los, pois eles já foram liberados ao liberar a lista. 
            freeLinkedList(&list);
            free(registersRemoved);

        }

        // Vai a para o inicio do arquivo e marca-o como consistente,
        head->status = '1';
        markConsistence(inputBin, head->status);

        // Libera o cabecalho.
        free(head);

        // Retorna quantos registros foram removidos.
        return numRegistersRemoved;

    }
}

// Insere um novo registro no arquivo.
int insertOnBin(FILE* targetfile, Register *newReg, int ignoreStatus) {

    // Garante que esteja no começo do arquivo.
    fseek(targetfile, 0, SEEK_SET);

    // Le o cabecalho.
    Header* head = readHeaderFromBin(targetfile, ignoreStatus);

    if(head == NULL || (head->status != '1' && ignoreStatus == 0)) { // Se o arquivo esta inconsistente ou o cabecalho nao pode ser gerado retorna.
        if(head != NULL)
            free(head);
        return -1;
    } else {

        // Vai a para o inicio do arquivo e marca-o como inconsistente,
        head->status = '0';
        markConsistence(targetfile, head->status);

        // Verifica se a lista encadeada do registro não está vazia.
        int inserted = 0;
        if(head->topoLista != -1) {

            // Carrega a lista encadeada.
            ListaEnc *list = createLinkedList();

            // Posiciona no primeiro elemento da lista encadeada.
            fseek(targetfile, head->topoLista, SEEK_SET);

            // Lê os registros da lista encadeada.
            Register *reg = NULL;
            do {

                // Lê o próximo registro do encadeamento.
                reg = readNextRegister(targetfile, head);

                if(reg == NULL) break;  // Verifica se a lista acabou.

                // Insere o registro na lista.
                insertLinkedListBack(list, reg);

                if(reg->encadeamentoLista == -1) break;  // Verifica se a lista acabou.

                // Posiciona no proximo elemento da lista encadeada.
                fseek(targetfile, reg->encadeamentoLista, SEEK_SET);

            } while(reg != NULL && reg->encadeamentoLista != -1);

            // Percorre a lista encadeda buscando um registro removido em que caiba o novo registro.
            Register *cur = NULL;
            Register *prev = NULL;

            // Insere o registro na primeira posição onde ele cabe.
            for(int i  = 0; i < getSizeLinkedList(list); i++) {

                prev = cur;
                cur = getLinkedList(list, i);

                // Insere o registro se ele cabe.
                if(cur->tamanhoRegistro >= newReg->tamanhoRegistro) {

                    // Insere o registro no primeiro campo onde ele cabe.
                    newReg->byteOffset = cur->byteOffset;
                    writeRegister(targetfile, head, newReg, 1);

                    inserted++;

                    break;

                }
            }

            // Atualiza a lista encadeada se o novo registro foi inserido em um de seus elementos.
            if(inserted != 0) {

                // Se foi inserido no primeiro registro atualiza topoLista.
                if(cur->byteOffset == head->topoLista) {

                    if(cur->encadeamentoLista == -1) {

                        long long int newEnc = -1;

                        fseek(targetfile, sizeof(char), SEEK_SET);
                        fwrite(&newEnc, sizeof(long long int), 1, targetfile);

                    } else {

                        fseek(targetfile, sizeof(char), SEEK_SET);
                        fwrite(&(cur->encadeamentoLista), sizeof(long long int), 1, targetfile);

                    }

                } else { // Se foi inserido no meio atualiza o registro anterior.

                    fseek(targetfile, prev->byteOffset + sizeof(char) + sizeof(int), SEEK_SET);
                    fwrite(&(cur->encadeamentoLista), sizeof(long long int), 1, targetfile);

                }
            }

            // Libera a memória da lista encadeda.
            freeLinkedList(&list);

        }

        if (!inserted) { // Se a lista está vazia ou o registro não cabe, insere no final do arquivo.

            // Verifica se o registro cabe nesta página de disco.
            fseek(targetfile, 0, SEEK_END);
            int pagePos = ftell(targetfile) % PAGE_SIZE;
            if(pagePos + sizeof(char) + sizeof(int) + newReg->tamanhoRegistro > PAGE_SIZE) // Se não preenche o resto da página com lixo e inicia uma nova.
                fillTrash(targetfile, TRASH_FILLER, PAGE_SIZE - pagePos);

            // Insere no final.
            newReg->byteOffset = ftell(targetfile);
            writeRegister(targetfile, head, newReg, 0);

        }

        // Vai a para o inicio do arquivo e marca-o como consistente.
        head->status = '1';
        markConsistence(targetfile, head->status);

        // Libera o cabecalho.
        free(head);

        return newReg->byteOffset;
    }
}


// Atualiza um valor de um registro.
int updateRegisterOnBin(FILE* targetfile, char* fieldName, char* fieldValue, char* updateFieldName, char* updateFieldValue) {

    // Garante que esteja no começo do arquivo.
    fseek(targetfile, 0, SEEK_SET);

    // Le o cabecalho.
    Header* head = readHeaderFromBin(targetfile, 0);

    // Guarda os registros a serem atualizados.
    Register **registersToUpdate = NULL;
    int numRegistersUpdated = 0;

    if(head == NULL || head->status != '1') { // Se o arquivo esta inconsistente ou o cabecalho nao pode ser gerado retorna.
        if(head != NULL)
            free(head);

        return -1;
    } else {

         Register* curRegister = NULL;
        int keepRegister; // Usado para marcar que o registro anterior deve ser mantido.
        do {

            // Libera a memoria do registro anterior se ele nao for mantido.
            if(curRegister != NULL && keepRegister == 0)
                curRegister = freeRegister(curRegister);
            keepRegister = 0;

            // Lê o próximo registro.
            curRegister = readNextRegister(targetfile, head);

            // Se a leitura falhar o arquivo acabou.
            if(curRegister == NULL) continue;

            // Se o registro não foi removido, verifica se é preciso atualiza-lo.
            if(curRegister->removido == '-') {

                if(cmpRegisterField(curRegister, fieldName, fieldValue) == 0) { // Verifica se esse registro é um alvo da remoção.

                    registersToUpdate = realloc(registersToUpdate, sizeof(Register*) * (numRegistersUpdated + 1));
                    registersToUpdate[numRegistersUpdated] = curRegister;
                    numRegistersUpdated++;

                    // Marca que o registra deve ser mantido na memória.
                    keepRegister = 1; 

                    // Se o campo a ser removido era do tipo idServidor, não é necessário continuar.
                    if(strcmp(fieldName, "idServidor\0") == 0) break;
                }
            }

        } while (curRegister != NULL);

        // Atualiza os registros.
        if(numRegistersUpdated > 0) {

            // Vai a para o inicio do arquivo e marca-o como inconsistente,
            head->status = '0';
            markConsistence(targetfile, head->status);
        
            // Atualiza cada registro.
            for(int i = 0; i < numRegistersUpdated; i++) {

                // Guarda o tamanho anterior do registro.
                int oldSize = registersToUpdate[i]->tamanhoRegistro;

                if(strcmp(updateFieldName, "idServidor") == 0) {
                    
                    if(strcmp(updateFieldValue, "\0") == 0) { // Verifica se o campo é nulo e atualiza com o valor para quando está vazio.
                        registersToUpdate[i]->salarioServidor = 0;
                    } else { // Se não for atualiza com o valor.
                        registersToUpdate[i]->salarioServidor = atof(updateFieldValue);
                    }

                } else if(strcmp(updateFieldName, "salarioServidor") == 0) { // Atualiza o campo no registro da MEMÓRIA.

                    if(strcmp(updateFieldValue, "\0") == 0) { // Verifica se o campo é nulo e atualiza com o valor para quando está vazio.
                        registersToUpdate[i]->salarioServidor = -1;
                    } else { // Se não for atualiza com o valor.
                        registersToUpdate[i]->salarioServidor = atof(updateFieldValue);
                    }
                    
                } else if(strcmp(updateFieldName, "telefoneServidor") == 0) {

                    if(strcmp(updateFieldValue, "\0") == 0) { // Verifica se o campo é nulo e atualiza com o valor para quando está vazio.

                        char phoneFiller[14] = "\0@@@@@@@@@@@@@";
                        for(int i = 0; i < 14; i++)
                            registersToUpdate[i]->telefoneServidor[i] = phoneFiller[i];

                    } else { // Se não for atualiza com o valor.
                        for(int i = 0; i < 14; i++)
                            registersToUpdate[i]->telefoneServidor[i] = updateFieldValue[i];
                    }

                } else if(strcmp(updateFieldName, "nomeServidor") == 0) {
                    
                    if(strcmp(updateFieldValue, "\0") == 0) { // Verifica se o campo é nulo e atualiza com o valor para quando está vazio.

                        if(registersToUpdate[i]->nomeServidor != NULL) {
                            free(curRegister->nomeServidor);
                            registersToUpdate[i]->nomeServidor = NULL;
                        }

                    } else { // Se não for atualiza com o valor.
                        
                        // Libera a memória do nome antigo.
                        if(registersToUpdate[i]->nomeServidor != NULL) {
                            free(registersToUpdate[i]->nomeServidor);
                            registersToUpdate[i]->nomeServidor = NULL;
                        }

                        // Copia o nome novo.
                        registersToUpdate[i]->nomeServidor = malloc(sizeof(char) * (strlen(updateFieldValue) + 1));
                        strcpy(registersToUpdate[i]->nomeServidor, updateFieldValue);

                    }

                } else if(strcmp(updateFieldName, "cargoServidor") == 0) {
                    
                    if(strcmp(updateFieldValue, "\0") == 0) { // Verifica se o campo é nulo e atualiza com o valor para quando está vazio.

                        if(registersToUpdate[i]->cargoServidor != NULL) {
                            free(registersToUpdate[i]->cargoServidor);
                            registersToUpdate[i]->cargoServidor = NULL;
                        }

                    } else { // Se não for atualiza com o valor.
                        
                        // Libera a memória do cargo antigo.
                        if(registersToUpdate[i]->cargoServidor != NULL) {
                            free(registersToUpdate[i]->cargoServidor);
                            registersToUpdate[i]->cargoServidor = NULL;
                        }

                        // Copia o nome novo.
                        registersToUpdate[i]->cargoServidor = malloc(sizeof(char) * (strlen(updateFieldValue) + 1));
                        strcpy(registersToUpdate[i]->cargoServidor, updateFieldValue);

                    }

                }

                // Calcula o tamanho do registro atualizado.
                registersToUpdate[i]->tamanhoRegistro = sizeof(long long int) + sizeof(int) + sizeof(double) + ( 14 * sizeof(char));

                if(registersToUpdate[i]->nomeServidor != NULL) {
                    registersToUpdate[i]->tamanhoRegistro += strlen(registersToUpdate[i]->nomeServidor) + 6;
                } 
                if(registersToUpdate[i]->cargoServidor != NULL) {
                    registersToUpdate[i]->tamanhoRegistro += strlen(registersToUpdate[i]->cargoServidor) + 6;
                }

                // Se o registro atualizado cabe no espaço do registro antigo:
                if(registersToUpdate[i]->tamanhoRegistro <= oldSize) {

                    // Apaga o conteúdo antigo, mas mantém o valor de removido e tamanho do registro.
                    fseek(targetfile, registersToUpdate[i]->byteOffset + sizeof(char) + sizeof(int), SEEK_CUR);
                    fillTrash(targetfile, TRASH_FILLER, oldSize);

                    // Volta ao incio do registro.
                    fseek(targetfile, registersToUpdate[i]->byteOffset, SEEK_CUR);

                    // Escreve o registro atualizado.
                    writeRegister(targetfile, head, registersToUpdate[i], 1);


                } else { // Se o registro não cabe:

                    // Salva o idServidor como texto em um buffer para ser passado para a função.
                    char buffer [65];
                    sprintf(buffer, "%d", registersToUpdate[i]->idServidor);

                    // Remove o registro, realiza a busca por idServidor, porque é um campo sempre presente e de valor único.
                    removeFromBin(targetfile, "idServidor", buffer, 1);

                    // Re-insere o registro.
                    insertOnBin(targetfile, registersToUpdate[i], 1);

                }
            }
        }

        // Libera os registros da memória.
        registersToUpdate = freeRegisterList(registersToUpdate, numRegistersUpdated);

        // Vai a para o inicio do arquivo e marca-o como consistente,
        head->status = '1';
        markConsistence(targetfile, head->status);

        // Libera o cabecalho.
        free(head);

        // Retorna quantos registros foram atualizados.
        return numRegistersUpdated;

    }
}