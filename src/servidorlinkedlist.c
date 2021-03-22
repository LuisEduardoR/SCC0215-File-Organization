// Trabalho 4 - SCC0215 - Organizacao de Arquivos
// Luis Eduardo Rozante de Freitas Pereira 

# include <stdlib.h>

# include "servidorlinkedlist.h"

// Nó da lista encadeda.
typedef struct node Node;

struct node {

    Register *content; 
    Node *next;

};

// Lista encadada.
struct listaEnc {

    Node *start; // Aponta para o inicio da lista.
    Node* cur; // Aponta para o último nó buscado da lista.

    long unsigned size; // Tamanho da lista.

    // Usado para guardar o último nó acessado e permitir iteração crescente em 0(1).
    Node* previous_access;
    long unsigned previous_access_num;

};

// Cria uma nova lista.
ListaEnc *createLinkedList() {

    ListaEnc *l = malloc(sizeof(ListaEnc));

    if(l == NULL) return NULL;

    l->start = NULL;
    l->cur = NULL;

    l->size = 0;

    l->previous_access = NULL;
    l->previous_access_num = -1;

    return l;

}

// Insere um novo nó na lista na primeira posição antes de outro nó com tamanho do registro maior ou igual a ele.
int insertLinkedListFE(ListaEnc *l, Register *reg) {

    if(l == NULL || reg == NULL)
        return 1;

    if(l->size == 0) {

        l->start = malloc(sizeof(Node));
        if(l->start == NULL) return 1;

        l->start->next = NULL;
        l->start->content = reg;

    } else {

        // Pega o inicio da lista.
        Node *cur = l->start;

        // Encontra a aposição onde o nó deve ser inserido.
        while(cur->next != NULL && cur->next->content->tamanhoRegistro < reg->tamanhoRegistro) {

            cur = cur->next;

        }

        // Cria o novo nó.
        Node *newNode = malloc(sizeof(Node));
        if(newNode == NULL) return 1;

        newNode->content = reg;

        // Insere no inicio.
        if(cur == l->start && reg->tamanhoRegistro <= l->start->content->tamanhoRegistro) {
                
                newNode->next = l->start;
                l->start = newNode;

        } else { // Insere no meio ou no final.

            newNode->next = cur->next;
            cur->next = newNode;

        }

    }

    // Reseta o nó salvo, pois ele pode ter sido movido.
    l->previous_access = NULL;
    l->previous_access_num = -1;

    l->size++;
    return 0;    

}

// Insere um novo nó na lista na última posição da lista.
int insertLinkedListBack(ListaEnc *l, Register *reg) {

    if(l == NULL || reg == NULL)
        return 1;

    if(l->size == 0) {

        l->start = malloc(sizeof(Node));
        if(l->start == NULL) return 1;

        l->start->next = NULL;
        l->start->content = reg;

    } else {

        // Pega o inicio da lista.
        Node *cur = l->start;

        // Encontra a aposição onde o nó deve ser inserido.
        while(cur->next != NULL) {

            cur = cur->next;

        }

        // Cria o novo nó.
        Node *newNode = malloc(sizeof(Node));
        if(newNode == NULL) return 1;

        newNode->content = reg;
        newNode->next = NULL;

        // Insere o nó.
        cur->next = newNode;


    }

    // Reseta o nó salvo, pois ele pode ter sido movido.
    l->previous_access = NULL;
    l->previous_access_num = -1;

    l->size++;
    return 0;    

}

// Retorna o conteúdo de um nó na lista.
Register *getLinkedList(ListaEnc *l, long unsigned pos) {

    // Se a lista é nula ou a posição é inválida retorna NULL.
    if(l == NULL || pos < 0 || pos >= l->size)
        return NULL;

    // Verifica se a posição do nó foi salva para iteração crescente rápida.
    if(l->previous_access_num != -1) {

        if(l->previous_access_num == pos) {

            // Retorna o conteúdo do nó salvo.
            return l->previous_access->content;

        } else if(l->previous_access_num + 1 == pos) {

            // Atualiza o último nó acessado.
            l->previous_access = l->previous_access->next;
            l->previous_access_num++;

            // Retorna o conteúdo do nó salvo.
            return l->previous_access->content;

        }
    }

    // Busca o nó.
    Node *cur_node = l->start;
    long unsigned cur = 0;

    while(cur < pos) {
        cur_node = cur_node->next;
        cur++;
    }

    // Atualiza o último nó acessado.
    l->previous_access = cur_node;
    l->previous_access_num = cur;

    // Retorna o conteúdo do nó.
    return cur_node->content;

}

// Retorna o tamanho da lsita encadeada.
long unsigned getSizeLinkedList(ListaEnc *l) {

    if(l == NULL) return -1; // Sim eu sei que é unsigned haha, estou colocando isso para ficar "infinito" =)

    return l->size;

}

// Libera a memória da lista encadeada e seu conteúdo.
void freeLinkedList(ListaEnc **l) {

    int nodes = 0;

    // Se a lista nao esta vazia.
    if((*l)->size > 0) {

        // Pega o inicio da lista.
        Node *cur = (*l)->start;

        // Enquanto não estiver no fim da lsita libera o nó e seu conteúdo e vai para o próximo.
        while (cur != NULL)
        {
            
            Node *temp = cur;

            cur = cur->next;

            freeRegister(temp->content);
            free(temp);
            nodes++;

        }

    }

    free(*l);
    *l = NULL;

}
