// Trabalho 4 - SCC0215 - Organizacao de Arquivos
// Luis Eduardo Rozante de Freitas Pereira 

# ifndef SERVIDOR_LINKED_LIST_H
# define SERVIDOR_LINKED_LIST_H

# include "servidorregister.h"

typedef struct listaEnc ListaEnc;

// Cria uma nova lista.
ListaEnc *createLinkedList();

// Insere um novo nó na lista na primeira posição antes de outro nó com tamanho do registro maior ou igual a ele.
int insertLinkedListFE(ListaEnc *l, Register *reg);

// Insere um novo nó na lista na última posição da lista.
int insertLinkedListBack(ListaEnc *l, Register *reg);

// Retorna o conteúdo de um nó na lista.
Register *getLinkedList(ListaEnc *l, long unsigned pos);

// Retorna o tamanho da lsita encadeada.
long unsigned getSizeLinkedList(ListaEnc *l);

// Libera a memória da lista encadeada e seu conteúdo.
void freeLinkedList(ListaEnc **l);

#endif

