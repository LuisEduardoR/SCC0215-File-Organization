// Trabalho 4 - SCC0215 - Organizacao de Arquivos
// Luis Eduardo Rozante de Freitas Pereira 

# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# include "servidorheader.h"
# include "servidorregister.h"

# include "filecreateprint.h"

# include "util.h"

// Subrotina que gera as tags para um header a partir das descricoes dos campos ou de chars pre-determinados.
void generateHeaderTags(Header* header, char* forcedTags) {

    // Gera as tags para os campos.
    for(int i = 0; i < 5; i++) {
        if(forcedTags != NULL && i < strlen(forcedTags))
                header->tagCampo[i] = forcedTags[i];
        else { // Gera tags a partir da descricao.

            header->tagCampo[i] = header->desCampo[i][0];        

            // Verifica se a tag gerada nao eh repetida.            
            char unique = 0;
            char startTag = header->tagCampo[i];
            while(!unique) {

                // Marca que a tag ainda nao foi encontrada repetindo.
                unique = 1;

                // Compara com as outras tags.
                for(int j = i - 1; j >= 0; j--) {

                    // Se a tag estiver repetida tenta uma nova tag.
                    if(header->tagCampo[i] == header->tagCampo[j]) {

                        header->tagCampo[i]++;
                        if(header->tagCampo[i] > 'Z' && header->tagCampo[i] < 'a')
                            header->tagCampo[i] = 'a';
                        else if(header->tagCampo[i] > 'z')
                            header->tagCampo[i] = 'A';

                        // Se todas as letras ja foram usadas a tag sera forcada a repetir.
                        if(startTag == header->tagCampo[i]) {
                            unique = 1;
                            break;
                        }

                        // Marca que a nova tag tem de ser verificada.
                        unique = 0;
                        break;

                    }
                }
            }
        }
    }
}

// Le e gera um header a partir de um arquivo .csv.
// OBS1: forcedTags sera usado para gerar as tags dos campos, onde o campo 1 recebera o char 1 e assim por diante, caso queira que tags sejam geradas
// automaticamente use NULL.
// OBS2: Essa funcao avancara a leitura do arquivo .csv em uma linha.
Header* readHeaderFromCsv(FILE* inputCsv, char* forcedTags) {

    // Aloca espaco para o cabecalho.
    Header* head = malloc(sizeof(Header));
    if(head == NULL) return NULL; // Retorna em caso de erro.
    
    // Guarda as variaves de status e topoLista.
    head->status = '0';
    head->topoLista = -1;

    // Buffer para guardar a linha que contem o cabecalho.
    char* buffer = NULL;
    size_t bufferSize = 0;

    // Le a primeira linha do arquivo csv como cabecalho.
    getline(&buffer, &bufferSize, inputCsv);

    // Le a descricao dos campos do arquivo .csv.
    sscanf(buffer, "%39[^,\n\t\r],%39[^,\n\t\r],%39[^,\n\t\r],%39[^,\n\t\r],%39[^,\n\t\r] ", head->desCampo[0], head->desCampo[1], head->desCampo[2], head->desCampo[3], head->desCampo[4]);

    // Libera o buffer.
    free(buffer);
    bufferSize = 0;

    // Gera as tags para os campos.
    generateHeaderTags(head, forcedTags);

    return head;

}

// Le e gera um header a partir de um arquivo binario.
Header* readHeaderFromBin(FILE* inputBin, int ignoreStatus) {

    // Aloca espaco para o header.
    Header* head = malloc(sizeof(Header));
    if(head == NULL) // Retorna um erro se a alocacao falhar.
        return NULL;

    // Garante que esteja no ínicio do arquivo.
    fseek(inputBin, 0, SEEK_SET);

    // Le o status do arquivo.
    fread(&(head->status), sizeof(char), 1, inputBin);

    if(head->status != '1' && ignoreStatus == 0) { // Retorna um erro caso o arquivo nao esteja marcado como consistente.
        free(head);
        return NULL;
    }

    // Le o encadeamento da lista.
    fread(&(head->topoLista), sizeof(long long int), 1, inputBin);

    // Le a tag e decricao dos campos para o header.
    for(int i = 0; i < 5; i++) {

        fread(&(head->tagCampo[i]), sizeof(char), 1, inputBin);
        fread(head->desCampo[i], sizeof(char), 40, inputBin);

    }

    // Seta o arquivo para o inicio da segunda pagina de disco.
    fseek(inputBin, PAGE_SIZE, SEEK_SET);

    // Retorna o header lido.
    return head;

}

// Escreve o caçalho em um arquivo binário. 
int writeHeaderToBin(Header* header, FILE* outputBin) {

    // Guarda o numero de bytes lidos.
    size_t bytesWritten = 0;

    // Garante que esteja no começo do arquivo.
    fseek(outputBin, 0, SEEK_SET);

    // Escreve o status e o topoLista no arquivo.
    bytesWritten += fwrite(&(header->status), sizeof(char), 1, outputBin) * sizeof(char);
    
    bytesWritten += fwrite(&(header->topoLista), sizeof(long long int), 1, outputBin) * sizeof(long long int);

    // Escreve a tag e descricao de cada campo no cabecalho, preenchendo qualquer espaco que sobrar na descricao com o caracter de lixo.
    for(int i = 0; i < 5; i++) {

        bytesWritten += fwrite(&(header->tagCampo[i]), sizeof(char), 1, outputBin) * sizeof(char); // Escreve a tag.

        // Escreve a descrição.
        int desLen = 1 + strlen(header->desCampo[i]); 
        bytesWritten += fwrite(&(header->desCampo[i]), sizeof(char), desLen, outputBin);

        fillTrash(outputBin, TRASH_FILLER, 40 - desLen); // Preenche o espaço que sobra com lixo.
        bytesWritten += 40 - desLen;

    }

    fillTrash(outputBin, TRASH_FILLER, PAGE_SIZE - bytesWritten);

    return 1;

}