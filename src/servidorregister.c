// Trabalho 4 - SCC0215 - Organizacao de Arquivos
// Luis Eduardo Rozante de Freitas Pereira 

# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# include "servidorregister.h"
# include "servidorheader.h"

# include "filecreateprint.h"

# include "util.h"

// Cria e inicializa um novo registro.
Register *createRegister(int id, double salario, char* telefone, char* nome, char* cargo) {

    Register *reg = NULL;

    // Aloca memoria para o novo registro e verifica se houveram erros.
    reg = malloc(sizeof(Register));
    if(reg == NULL) return NULL;

    // Copia os campos de tamanho fixo.
    reg->removido = '@';
    reg->encadeamentoLista = -1;
    reg->byteOffset = -1;

    reg->idServidor = id;
    reg->salarioServidor = salario;

    for(int i = 0; i < 14; i++) {
        reg->telefoneServidor[i] = telefone[i];
    }

    // Conta os campos de tamanho fixo para o tamanho do registro.
    reg->tamanhoRegistro = sizeof(long long int) + sizeof(int) + sizeof(double) + ( 14 * sizeof(char));


    // Copia os campos de tamanho variável.
    if(nome != NULL) {

        int size = strlen(nome) + 1;
        reg->nomeServidor = malloc(sizeof(char) * size);
        strcpy(reg->nomeServidor, nome);

    } else reg->nomeServidor = NULL;

    if(nome != NULL) {

        int size = strlen(cargo) + 1;
        reg->cargoServidor = malloc(sizeof(char) * size);
        strcpy(reg->cargoServidor, cargo);

    } else reg->cargoServidor = NULL;
    
    // Adiciona os campos de tamanho variável ao tamanho.
    if(reg->nomeServidor != NULL) {
        reg->tamanhoRegistro += strlen(reg->nomeServidor) + 6;
    } 
    if(reg->cargoServidor != NULL) {
        reg->tamanhoRegistro += strlen(reg->cargoServidor) + 6;
    }

    return reg;

}

// Cria e inicializa um novo registro vazio.
Register *createEmptyRegister() {

    Register *reg = NULL;

    // Aloca memoria para o novo registro e verifica se houveram erros.
    reg = malloc(sizeof(Register));
    if(reg == NULL) return NULL;

    // Inicializa o registro
    reg->removido = '@';
    reg->tamanhoRegistro = -1;
    reg->encadeamentoLista = -1;

    reg->idServidor = 0;
    reg->salarioServidor = -1;

    char fillPhone[14] = "\0@@@@@@@@@@@@@";

    for(int i = 0; i < 14; i++)
    reg->telefoneServidor[i] = fillPhone[i];

    reg->nomeServidor = NULL;
    reg->cargoServidor = NULL;

    reg->byteOffset = -1;

    return reg;

}

// Gera um arquivo binario com registros a partir de um arquiv .csv.
Register **readRegisterFromCsv(Register **registers, int *numRegisters, FILE *inputCsv) {

    char* buffer = NULL;

    // Le dados do arquivo de entrada e cria uma lista com os registros a serem salvos.
    int curField = 1;
    while(1){

        // Le os campos para o registro.
        if(curField <= 5) {

            // Le um campo para o buffer e verifica se houve sucesso na leitura.
            int readSuccess = fscanf(inputCsv, "%m[^,\n\t\r]", &buffer);

            // Remove caracteres indesejados.
            if(curField != 5)
                fscanf(inputCsv, "%*c"); // Remove a virgula.
            else
                fscanf(inputCsv, " "); // Remove o \n e vai para a próxima linha.

            if(curField == 1) { // Le o id do servidor campo.
                
                // Se o id nao pode ser lido o arquivo acabou ou eh invalido.
                if(readSuccess < 1) 
                    break;

                // Como esse eh o primeiro campo de um novo resitro, aloca espaco na memoria.
                registers = realloc(registers, ((*numRegisters) + 1) * sizeof(Register*));

                registers[*numRegisters] = NULL;
                registers[*numRegisters] = malloc(sizeof(Register));

                // Converte o valor do buffer para int e guarda no registro.
                registers[*numRegisters]->idServidor = atoi(buffer);   

                // Limpa o buffer.
                free(buffer); 
                buffer = NULL;

            } else if(curField == 2) { // Le o salario do servidor campo.
                
                if(readSuccess < 1)
                    registers[*numRegisters]->salarioServidor = 0; // Se o valor nao pode ser lido guarda 0 no registro.
                else
                    registers[*numRegisters]->salarioServidor = atof(buffer); // Converte o valor do buffer para double e guarda no registro.
                
                // Limpa o buffer.
                free(buffer);
                buffer = NULL;  

            } else if(curField == 3) { // Le o telefone do servidor campo.

                // Preenche o telefone do servidor com uma string vazia seguida por lixo se a leitura falho.
                if(readSuccess < 1) {

                    registers[*numRegisters]->telefoneServidor[0] = '\0';
                    for(int i = 1; i < 14; i++)
                            registers[*numRegisters]->telefoneServidor[i] = '@';
                    
                } else {

                    for(int i = 0; i < 14; i++) // Copia o telefone para o registro.
                        registers[*numRegisters]->telefoneServidor[i] = buffer[i];

                }

                // Limpa o buffer.
                free(buffer);
                buffer = NULL;

            } else if(curField == 4) { // Le o nome do servidor.
                
                if(readSuccess < 1) {

                    registers[*numRegisters]->nomeServidor = NULL; // Indica que o nome do servidor nao pode ser lido.

                } else { 

                    registers[*numRegisters]->nomeServidor = buffer; // "Move" o endereco do buffer para o registro, efetivamente copiando a string, e
                    buffer = NULL;                                   // marca o buffer como NULL para evitar que o campo dentro do registro possa ser
                                                                     // afetado por acoes no buffer.

                }   

            } else if(curField == 5) {
                
                if(readSuccess < 1) {

                    registers[*numRegisters]->cargoServidor = NULL;

                } else { 
                    
                   registers[*numRegisters]->cargoServidor = buffer; // "Move" o endereco do buffer para o registro, efetivamente copiando a string, e
                    buffer = NULL;                                   // marca o buffer como NULL para evitar que o campo dentro do registro possa ser
                                                                     // afetado por acoes no buffer.


                } 
            }

            curField++; // Vai para o proximo campo.

        } else {

            // Copia os campos com informacao sobre o registro.
            registers[*numRegisters]->removido = '-';
            registers[*numRegisters]->encadeamentoLista = -1;

            // > Calcula o tamanho do registro.
            int tamReg = sizeof(long long int) + sizeof(int) + sizeof(double) + ( 14 * sizeof(char));
            if(registers[*numRegisters]->nomeServidor != NULL) {
                tamReg += strlen(registers[*numRegisters]->nomeServidor) + 6;
            } if(registers[*numRegisters]->cargoServidor != NULL) {
                tamReg += strlen(registers[*numRegisters]->cargoServidor) + 6;
            }
            
            registers[*numRegisters]->tamanhoRegistro = tamReg;
            
            // Prepara-se para ler o proximo registro.
            (*numRegisters)++;
            curField = 1;

        }
    }

    // Limpa o buffer se ele ja nao estiver vazio.
    if(buffer != NULL)
        free(buffer);

    return registers;

}

// Salva os registros para um arquivo binario.
int writeRegistersToFile(FILE* outputBin, Header *header, Register **registers, int numRegisters) {

    // Garante que se esteja no comeco da primeira pagina de disco depois do cabecalho.
    fseek(outputBin, PAGE_SIZE, SEEK_SET);

    // Escreve os registros no arquivo.
    for(int i = 0; i < numRegisters; i++) {

        // Guarda os bytes que sobram no final de uma página de disco para serem adicionados ao tamanho do registro.
        int diskPageRemainder = 0;

        // Verifica se esse registro cabe na pagina de disco.
        if(i > 1) {
        
            // Se não cabe preenche o restante da página com lixo e atualiza o tamanho do registro anterior para incluir o lixo.
            int tamCur = registers[i]->tamanhoRegistro + (sizeof(char) + sizeof(int));
            if((ftell(outputBin) % PAGE_SIZE) + tamCur > PAGE_SIZE) {

                // Marca o offsetAtual para retornar depois.
                long long int curOffset = ftell(outputBin);      

                // Calcula a quantidade de lixo a ser colocada na página.
                int trashAmount = PAGE_SIZE - ((curOffset % PAGE_SIZE));

                // Adiciona o lixo ao registro anterior.
                registers[i-1]->tamanhoRegistro += trashAmount;  
                fseek(outputBin, registers[i-1]->byteOffset + sizeof(char), SEEK_SET);
                fwrite(&(registers[i-1]->tamanhoRegistro), sizeof(int), 1, outputBin);

                // Retorna para a posição.
                fseek(outputBin, curOffset, SEEK_SET);

                // Se o registro nao cabe na pagina, preenche o restante dela com o caracter de lixo e inicia uma nova.
                fillTrash(outputBin, TRASH_FILLER, trashAmount);

            }

        }

        // Escreve o registro no arquivo.
        registers[i]->byteOffset = ftell(outputBin); // Marca o offset onde o registro será escrito.
        registers[i]->tamanhoRegistro += diskPageRemainder; // Adicona os bytes que sobram na página de disco se houverem.
        writeRegister(outputBin, header, registers[i], 0);
        
    }

    // Calcula a quantidade de páginas de disco usadas sem o cabeçalho.
    int pagesUsed = (ftell(outputBin) / PAGE_SIZE) - 1;
    if(ftell(outputBin) % PAGE_SIZE != 0) pagesUsed++; // Conta a página incompleta que pode sobrar no final.

    // Marca que o arquivo foi salvo com sucesso.
    int status = '1';
    fseek(outputBin, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, outputBin);

    // Retorna o numero de paginas de disco utilizadas (sem o cabecalho).
    return ftell(outputBin);

}

// Le o proximo registro de um arquivo binario e o retorna.
Register* readNextRegister(FILE* inputBin, Header *header) {

    // Guarda o byteOffset, usado para calcular a posicao do proximo registro.
    long long int byteOffset = ftell(inputBin);
    // Salva o byte offset do inicio do registro para poder guardar nele depois que "byteOffset" já foi alterado.
    long long int byteOffsetStart = byteOffset;

    // Guarda se o registro foi removido.
    char removed = 0;
    int registerSize = 0; // Guarda o tamanho do registro.

    // Le se o registro foi removido.
    int r = fread(&removed, sizeof(char), 1, inputBin) * sizeof(char);
    byteOffset += r; // Incrementa o numero de bytes lidos.

    // Se nao foi posivel ler se o registro foi removido, entao o arquivo acabou.
    if(r == 0) return NULL;

    // Le o tamanho do registro.
    byteOffset += fread(&registerSize, sizeof(int), 1, inputBin) * sizeof(int);

    // Cria um novo registro.
    Register *reg = NULL;
    reg = createEmptyRegister();

    // Copia os dados de tamanho fixo do registro do arquivo para o da memoria.
    reg->removido = removed;
    reg->tamanhoRegistro = registerSize;

    // Salva o byteOffset do inicio do registro, usado para facilitar algumas funções.
    reg->byteOffset = byteOffsetStart;

    // Le o campo lista encadeada para o registro.
    fread(&(reg->encadeamentoLista), sizeof(long long int), 1, inputBin);  

    if(removed == '-') { // Se o registro eh um registro valido que nao foi removido:

        // Copia os campos de tamanho fixo para a memoria.
        fread(&(reg->idServidor), sizeof(int), 1, inputBin);
        fread(&(reg->salarioServidor), sizeof(double), 1, inputBin);
        fread(reg->telefoneServidor, sizeof(char), 14, inputBin);

        // Copia os dados de tamanho variavel.

        // Variaveis auxiliares para guardar o tamanho dos campos.
        int tam1 = 0;
        int tam2 = 0;

        // Verifica se existe pelo menos um campo de tamanho variavel.
        if(registerSize > 34) {

            // Obtem o tamanho do campo.
            fread(&tam1, sizeof(int), 1, inputBin);
            // Detecta o tipo do campo.
            char field;
            fread(&field, sizeof(char), 1, inputBin);

            // Copia o campo para o lugar apropriado.
            if(field == header->tagCampo[3]) {
                reg->nomeServidor = malloc(sizeof(char) * (tam1 - 1));
                fread(reg->nomeServidor, sizeof(char), tam1 - 1, inputBin);
            } else if(field == header->tagCampo[4]) {
                reg->cargoServidor = malloc(sizeof(char) * (tam1 - 1));
                fread(reg->cargoServidor, sizeof(char), tam1 - 1, inputBin);
            }
        }

        // Verifica se existe um segundo campo de tamanho variavel.
        if(registerSize > (38 + tam1)) {
            
            // Obtem o tamanho do campo.
            fread(&tam2, sizeof(int), 1, inputBin);
            // Detecta o tipo do campo.
            char field;
            fread(&field, sizeof(char), 1, inputBin);

            // Copia o campo para o lugar apropriado.
            if(field == header->tagCampo[3]) {
                reg->nomeServidor = malloc(sizeof(char) * (tam2 - 1));
                fread(reg->nomeServidor, sizeof(char), tam2 - 1, inputBin);
            } else if(field == header->tagCampo[4]) {
                reg->cargoServidor = malloc(sizeof(char) * (tam2 - 1));
                fread(reg->cargoServidor, sizeof(char), tam2 - 1, inputBin);
            }
        }
        
    }

    // Atualiza o byteOffset;
    byteOffset += registerSize;

    // Posiciona o arquivo no inicio do proximo registro.
    fseek(inputBin, byteOffset, SEEK_SET);

    return reg;

}

// Remove um registrador do arquivo binario. OBS: essa função não refaz o encadeamneto da lista, este deve ser refeito depois.
int removeRegister(FILE* targetfile, Header *header, Register *reg) {

    // Verifica se o registro tem um byteOffset válido.
    if(reg->byteOffset < 0)
        return 1;

    char removido = '*';

    // Salva o offset antes da operação para recupera-lo no final.
    int prevOffset = ftell(targetfile);

    // Vai para o inicio do registro aser removido.
    fseek(targetfile, reg->byteOffset, SEEK_SET);

    // Marca que o registro foi removido.
    fwrite(&removido, sizeof(char), 1, targetfile);

    // Vai para logo depois do encadeamentoLista.
    fseek(targetfile, sizeof(int) + sizeof(long long int), SEEK_CUR);

    // Preenche o registro removido com lixo.
    fillTrash(targetfile, TRASH_FILLER, reg->tamanhoRegistro - sizeof(long long int));

    // Retorna o arquivo para a posição anterior a recomoção.
    fseek(targetfile, prevOffset, SEEK_SET);

    return 0;

}

// Insere um novo registro em uma determinada posição no arquivo, se recycling é verdadeiro, reutiliza um espaço previamente 
// removido (não rescreve o tamanhoRegistro).
int writeRegister(FILE* targetfile, Header *header, Register *reg, int recycling) {

    // Vai para a posição correta.
    fseek(targetfile, reg->byteOffset, SEEK_SET);

    reg->removido = '-'; // Marca que o registro está no arquivo na memória.

    // Escreve que o registro não foi removido no arquivo.
    fwrite(&(reg->removido), sizeof(char), 1, targetfile);

    if(recycling) {

        // Pula o tamanho do registro no arquivo.
        fseek(targetfile, sizeof(int), SEEK_CUR);

    } else {

        // Escreve o tamanho do registro no arquivo.
        fwrite(&(reg->tamanhoRegistro), sizeof(int), 1, targetfile);

    }

    reg->encadeamentoLista = -1; // Marca que o registro não faz mais parte do encadeamento na memória.
    // Escreve o encadeamento do registro no arquivo.
    fwrite(&(reg->encadeamentoLista), sizeof(long long int), 1, targetfile);

    // Escreve os campos de tamanho fixo no arquivo.
    fwrite(&(reg->idServidor), sizeof(int), 1, targetfile);
    fwrite(&(reg->salarioServidor), sizeof(double), 1, targetfile);
    fwrite(reg->telefoneServidor, sizeof(char), 14, targetfile);

    // Escreve os campos de tamanho variavel no arquivo.

    if(reg->nomeServidor != NULL) { // Escreve o tamanho, a tag e o conteudo do campo nomeServidor se ele nao estiver vazio.
    
        char tagC4 = header->tagCampo[3]; // Obtem a tag do campo.
        int tamC4 = strlen(reg->nomeServidor) + 2; // Obtem o tamanho do campo.

        // Escreve no arquivo.
        fwrite(&tamC4, sizeof(int), 1, targetfile);
        fwrite(&tagC4, sizeof(char), 1, targetfile);
        if(tamC4 > 0)
            fwrite(reg->nomeServidor, sizeof(char), tamC4 - 1, targetfile);

    }

    if(reg->cargoServidor != NULL) { // Escreve o tamanho, a tag e o conteudo do campo cargoServidor se ele nao estiver vazio.
    
        char tagC5 = header->tagCampo[4]; // Obtem a tag do campo.
        int tamC5 = strlen(reg->cargoServidor) + 2; // Obtem o tamanho do campo.

        // Escreve no arquivo.
        fwrite(&tamC5, sizeof(int), 1, targetfile);
        fwrite(&tagC5, sizeof(char), 1, targetfile);
        if(tamC5 > 0)
            fwrite(reg->cargoServidor, sizeof(char), tamC5 - 1, targetfile);

    }

    return 0;
    
}

// Imprime o conteudo de um registro para a saida.
void printRegister(FILE* output, Register *reg) {

        if(reg == NULL) {
            fprintf(output, "Registro inexistente.\n");
            return;
        }

        fprintf(output, "%d", reg->idServidor);

        if(reg->salarioServidor != -1)
            fprintf(output, " %.2lf", reg->salarioServidor);
        else
            fprintf(output, "         ");
        
        if(reg->telefoneServidor[0] != '\0') {
            fprintf(output, "%c", ' ');
            for(int i = 0; i < 14; i++)
                fprintf(output, "%c", reg->telefoneServidor[i]);
        } else {
            fprintf(output, "%s" , "               ");
        }

        if(reg->nomeServidor != NULL) {

            fprintf(output, " %d ", (int)strlen(reg->nomeServidor));
            fprintf(output, "%s", reg->nomeServidor);

        }

        if(reg->cargoServidor != NULL) {

            fprintf(output, " %d ", (int)strlen(reg->cargoServidor));
            fprintf(output, "%s", reg->cargoServidor);

        } 


        fprintf(output, "%c", '\n');
}

// Imprime o conteudo detalhado de um registro, incluindo o nome dos campos para a saida.
void printRegisterDetail(FILE* output, Header* header, Register *reg) {

        if(reg == NULL) {
            fprintf(output, "Registro inexistente.\n");
            return;
        }

        fprintf(output, "%s: %d\n", header->desCampo[0], reg->idServidor);

        if(reg->salarioServidor != -1)
            fprintf(output, "%s: %.2lf\n", header->desCampo[1], reg->salarioServidor);
        else
            fprintf(output, "%s: valor nao declarado\n" , header->desCampo[1]);
        
        if(reg->telefoneServidor[0] != '\0') {
            fprintf(output, "%s: ", header->desCampo[2]);
            for(int i = 0; i < 14; i++)
                fprintf(output, "%c", reg->telefoneServidor[i]);
            fprintf(output, "\n");
        } else
            fprintf(output, "%s: valor nao declarado\n" , header->desCampo[2]);

        if(reg->nomeServidor != NULL)
            fprintf(output, "%s: %s\n", header->desCampo[3], reg->nomeServidor);
        else
            fprintf(output, "%s:  valor nao declarado\n" , header->desCampo[3]);

        if(reg->cargoServidor != NULL)

            fprintf(output, "%s: %s\n", header->desCampo[4], reg->cargoServidor);
        else
            fprintf(output, "%s:  valor nao declarado\n" , header->desCampo[4]);


        fprintf(output, "%c", '\n');
}

// Calcula o tamanho do registro (sem os campos removido e tamanhoRegistro).
int getTamanhoRegistro(Register *reg) {

    if(reg == NULL) return -1;

    // Recalcula o tamanho do registro (necessário para descartar o lixo que pode ter sido inserido nos finais de página de disco).
    int tamReg = sizeof(long long int) + sizeof(int) + sizeof(double) + (14 * sizeof(char));
    if(reg->nomeServidor != NULL) {
        tamReg += strlen(reg->nomeServidor) + 6;
    } if(reg->cargoServidor != NULL) {
        tamReg += strlen(reg->cargoServidor) + 6;
    }

    return tamReg;

}

// Libera a memoria usada para guardar um registro.
Register *freeRegister(Register *reg) {

    if(reg != NULL) {

        if(reg->nomeServidor != NULL)
            free(reg->nomeServidor);
        if(reg->cargoServidor != NULL)
            free(reg->cargoServidor);

        free(reg);

    }

    return NULL;

}

// Libera uma lista dinamicamente alocada de registros.
Register **freeRegisterList(Register **regs, size_t size) {

    if(regs != NULL) {

        for(size_t i = 0; i < size; i++) freeRegister(regs[i]);
        free(regs);

    }

    return NULL;

}

// Função de comparação para ordenar registros por tamanho.
int cmpRegisterSize (const void *a, const void *b) {

    Register *ptr1 = *(Register * const *)a;
    Register *ptr2 = *(Register * const *)b;

    return  (ptr2->tamanhoRegistro - ptr1->tamanhoRegistro);

}

// Função de comparação para ordenar registros por idServidor.
int cmpRegisterId (const void *a, const void *b) {

    Register *ptr1 = *(Register * const *)a;
    Register *ptr2 = *(Register * const *)b;

    return  (ptr1->idServidor - ptr2->idServidor);

}

// Função de comparação para ordenar registros por nomeServidor usnado byteOffset como desempate.
int cmpRegisterNomeByte (const void *a, const void *b) {

    Register *ptr1 = *(Register * const *)a;
    Register *ptr2 = *(Register * const *)b;

    int diff = strcmp(ptr1->nomeServidor, ptr2->nomeServidor);
    if(diff == 0) {
        return (ptr1->byteOffset - ptr2->byteOffset);
    }
    return diff;

}

// Compara um determinado campo do registro do servidor com um determinado valor.
int cmpRegisterField(Register *reg, char* fieldName, char* fieldValue) {

    // Encontra o campo alvo.
    int targetField = -1;
    if(strcmp(fieldName, "idServidor") == 0) targetField = 1;
    else if(strcmp(fieldName, "salarioServidor") == 0) targetField = 2;
    else if(strcmp(fieldName, "telefoneServidor") == 0) targetField = 3;
    else if(strcmp(fieldName, "nomeServidor") == 0) targetField = 4;
    else if(strcmp(fieldName, "cargoServidor") == 0) targetField = 5;

    // Se o campo alvo nao for encontrado retorna.
    if(targetField == -1) return 0;

    // Verifica se o campo do registro contem o valor passado.
    if(targetField == 1) { // Compara o campo idServidor.

        if(strcmp(fieldValue, "\0") == 0) { // Verifica se o campo esta ausente.

            if(reg->idServidor == -1) // Compara o id do servidor.
                return 1;
            else
                return 0;  

        } else {

            int target = atoi(fieldValue); // Converte a string passada para int.
            return reg->idServidor - target; // Compara o id do servidor.

        }    

    } else if(targetField == 2) { // Compara o campo salarioServidor.

        if(strcmp(fieldValue, "\0") == 0) { // Verifica se o campo esta ausente.

            if(reg->salarioServidor == -1) // Compara o salario do servidor.
                return 1;
            else
                return 0;  

        } else {

            double target = atof(fieldValue); // Converte a string passada para double.
            return reg->salarioServidor - target; // Compara o salario do servidor.
                
        }

    } else if(targetField == 3) { // Compara o campo telefoneServidor.

        if(strcmp(fieldValue, "\0") == 0) { // Verifica se o campo está ausente.

            char phoneFill[14] = "\0@@@@@@@@@@@@@";

            for(int j = 0; j <= 14; j++) { // Compara o numero de telefone.
                if(j < 14) {

                    if(reg->telefoneServidor[j] != phoneFill[j]) // Se um caracter é diferente então o telefone não é nulo.
                        return 1;

                } else
                    return 0; // Os telefones são iguais.
            } 

        } else {

            for(int j = 0; j <= 14; j++) { // Compara o numero de telefone.
                if(j < 14) {

                    if(reg->telefoneServidor[j] > fieldValue[j]) // Se um caracter é maior então o telefone é têm um número maior.
                        return 1;
                    else if(reg->telefoneServidor[j] < fieldValue[j]) // Se um caracter é menor então o telefone é têm um número maior.
                        return -1;

                } else
                    return 0; // Os telefones são iguais.
            }   

        }

    } else if(targetField == 4) {

        if(strcmp(fieldValue, "\0") == 0) { // Verifica se o campo esta ausente.

            if(reg->nomeServidor == NULL) return 1;
            else return 0;

        } 
        
        if(reg->nomeServidor == NULL) return -1;

        return strcmp(reg->nomeServidor, fieldValue); // Compara o nome do servidor.

    } else if(targetField == 5) {

        if(strcmp(fieldValue, "\0") == 0) { // Verifica se o campo esta ausente.

            if(reg->cargoServidor == NULL) return 1;
            else return 0;

        } 
        
        if(reg->cargoServidor == NULL) return -1;

        return strcmp(reg->cargoServidor, fieldValue); // Compara o cargo do servidor.

    }

    return 0;

}