#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { RED, BLACK } Colore;

typedef struct Veicolo Veicolo;

struct Veicolo {
    int autonomia;
    int counter;
    Colore color;
    struct Veicolo* sx;
    struct Veicolo* dx;
    struct Veicolo* p;
};

typedef struct Stazione Stazione;

struct Stazione {
    int distanza;
    Veicolo* veicoli;
    Colore color;
    struct Stazione* sx;
    struct Stazione* dx;
    struct Stazione* p;
};

Stazione * autostrada = NULL;

struct Node {
    int data;
    struct Node* next;
    struct Node* parent;
};

struct LinkedList {  // liste per rappresentare il mio albero generalizzato
    struct Node* head;
};

struct Graph {
    int numLists;
    struct LinkedList** listArray; // Array di puntatori a liste
};

typedef struct ListNode {
    Stazione * stazione;
    struct ListNode* next;
} ListNode;


typedef struct DoublyLinkedList {  // rappresentano le due liste di appoggio per espandere i nodi
    int size;
    ListNode* head;
    ListNode* tail;
} DoublyLinkedList;

Veicolo* createNode(int key) {
    Veicolo* newNode = (Veicolo*)malloc(sizeof(Veicolo));
    newNode->autonomia = key;
    newNode->counter = 1;
    newNode->color = RED;
    newNode->sx = NULL;
    newNode->dx = NULL;
    newNode->p = NULL;
    return newNode;
}

void leftRotate(Veicolo** tree, Veicolo* x) {
    Veicolo* y = x->dx;
    x->dx = y->sx;
    if (y->sx != NULL)
        y->sx->p = x;
    y->p = x->p;
    if (x->p == NULL)
        *tree = y;
    else if (x == x->p->sx)
        x->p->sx = y;
    else
        x->p->dx = y;
    y->sx = x;
    x->p = y;
}

void leftRotateStazione(Stazione** tree, Stazione* x) {
    if (x == NULL || x->dx == NULL) {
        return;
    }
    Stazione* y = x->dx;
    x->dx = y->sx;
    if (y->sx != NULL) {
        y->sx->p = x;
    }
    y->p = x->p;
    if (x->p == NULL) {
        *tree = y;
    } else if (x == x->p->sx) {
        x->p->sx = y;
    } else {
        x->p->dx = y;
    }
    y->sx = x;
    x->p = y;
}

void rightRotate(Veicolo** tree, Veicolo* y) {
    Veicolo* x = y->sx;
    y->sx = x->dx;
    if (x->dx != NULL)
        x->dx->p = y;
    x->p = y->p;
    if (y->p == NULL)
        *tree = x;
    else if (y == y->p->sx)
        y->p->sx = x;
    else
        y->p->dx = x;
    x->dx = y;
    y->p = x;
}

void rightRotateStazione(Stazione** tree, Stazione* y) {
    if (y == NULL || y->sx == NULL) {
        return;
    }
    Stazione* x = y->sx;
    y->sx = x->dx;
    if (x->dx != NULL) {
        x->dx->p = y;
    }
    x->p = y->p;
    if (y->p == NULL) {
        *tree = x;
    } else if (y == y->p->sx) {
        y->p->sx = x;
    } else {
        y->p->dx = x;
    }
    x->dx = y;
    y->p = x;
}

// Inserimento di un Veicolo nell'albero rosso-nero
void rbInsert(Veicolo** tree, int key) {
    Veicolo* newNode = createNode(key);
    if (*tree== NULL) {
        *tree= newNode;
        (*tree)->color = BLACK;
        return;
    }

    Veicolo* current = *tree;
    Veicolo* padre = NULL;
    while (current != NULL) {
        padre = current;
        if (key < current->autonomia)
            current = current->sx;
        else if (key > current->autonomia)
            current = current->dx;
        else {
            // Nodo con stesso valore, incrementa il contatore e termina
            current->counter++;
            free(newNode);
            return;
        }
    }

    // Collega il nuovo nodo al padre
    newNode->p = padre;
    if(padre == NULL)
        *tree = newNode;
    else if (key < padre->autonomia)
        padre->sx = newNode;
    else
        padre->dx = newNode;

    // operazioni di bilanciamento dell'albero (il solito RB FIXUP)
    Veicolo* x = newNode;
    while (x->p != NULL && x->p->color == RED) {
        if (x->p == x->p->p->sx) {
            Veicolo* y = x->p->p->dx;
            if (y != NULL && y->color == RED) {
                x->p->color = BLACK;
                y->color = BLACK;
                x->p->p->color = RED;
                x = x->p->p;
            } else {
                if (x == x->p->dx) {
                    x = x->p;
                    leftRotate(tree, x);
                }
                x->p->color = BLACK;
                x->p->p->color = RED;
                rightRotate(tree, x->p->p);
            }
        } else {
            Veicolo* y = x->p->p->sx;
            if (y != NULL && y->color == RED) {
                x->p->color = BLACK;
                y->color = BLACK;
                x->p->p->color = RED;
                x = x->p->p;
            } else {
                if (x == x->p->sx) {
                    x = x->p;
                    rightRotate(tree, x);
                }
                x->p->color = BLACK;
                x->p->p->color = RED;
                leftRotate(tree, x->p->p);
            }
        }
    }
    (*tree)->color = BLACK;
}

//funzione d'appoggio per bilanciare l'albero dei veicoli
void rbTransplant(Veicolo** T, Veicolo * u, Veicolo * v) {
    if (u->p == NULL)
        *T = v;
    else if (u == u->p->sx)
        u->p->sx = v;
    else
        u->p->dx = v;

    if (v != NULL)
        v->p = u->p;
}
//funzione d'appoggio per bilanciare l'albero delle stazioni
void rbTransplantStazione(Stazione** tree, Stazione* u, Stazione* v) {
    if (u->p == NULL)
        *tree = v;
    else if (u == u->p->sx)
        u->p->sx = v;
    else
        u->p->dx = v;

    if (v != NULL)
        v->p = u->p;
}

void rbInsertFixupStazione(Stazione** tree, Stazione* z) {
    while (z != *tree && z->p != NULL && z->p->p != NULL && z->p->color == RED) {
        if (z->p == z->p->p->sx) {
            Stazione* y = z->p->p->dx;

            // metto controllo se no SEGM FAULT
            if (y != NULL && y->color == RED) { // Caso 1
                z->p->color = BLACK;
                y->color = BLACK;
                z->p->p->color = RED;
                z = z->p->p;
            } else { // Caso 2
                if (z->p->dx == z) {
                    z = z->p;
                    leftRotateStazione(tree, z);
                }
                // Caso 3
                if (z->p != NULL) {
                    z->p->color = BLACK;
                    if (z->p->p != NULL) {
                        z->p->p->color = RED;
                        rightRotateStazione(tree, z->p->p);
                    }
                }
            }
        } else {
            Stazione* y = z->p->p->sx;

            if (y != NULL && y->color == RED) { // Caso 1
                z->p->color = BLACK;
                y->color = BLACK;
                z->p->p->color = RED;
                z = z->p->p;
            } else { // Caso 2
                if (z->p->sx == z) {
                    z = z->p;
                    rightRotateStazione(tree, z);
                }
                // Caso 3
                if (z->p != NULL) {
                    z->p->color = BLACK;
                    if (z->p->p != NULL) {
                        z->p->p->color = RED;
                        leftRotateStazione(tree, z->p->p);
                    }
                }
            }
        }
    }

    if (*tree != NULL) {
        (*tree)->color = BLACK;
    }
}


void rbInsertStazione(Stazione** tree, Stazione* z) {
    Stazione* y = NULL;
    Stazione* x = *tree;

    while (x != NULL) {
        y = x;
        if (z->distanza < x->distanza)
            x = x->sx;
        else
            x = x->dx;
    }
    z->p = y;

    if (y == NULL)
        *tree = z;
    else if (z->distanza < y->distanza)
        y->sx = z;
    else
        y->dx = z;

    z->sx = z->dx = NULL;
    z->color = RED; //nodo inserito sempre rosso
    rbInsertFixupStazione(tree, z); // bilancia

    printf("aggiunta\n");
}

// Cercare un nodo nell'albero
Veicolo* findNode(Veicolo* root, int key) {
    if (root == NULL || root->autonomia == key)
        return root;
    if (key < root->autonomia)
        return findNode(root->sx, key);
    return findNode(root->dx, key);
}


void freeTree(Veicolo* root) {
    if (root != NULL) {
        freeTree(root->sx);
        freeTree(root->dx);
        free(root);
    }
}

// Funzione per liberare la memoria occupata dall'albero dei veicoli
void rbFree(Veicolo** tree) {
    freeTree(*tree);
    *tree = NULL;
}


// Valore minimo nell'albero
Veicolo* findMinimum(Veicolo* node) {
    if (node == NULL)
        return NULL;

    while (node->sx != NULL) {
        node = node->sx;
    }

    return node;
}

Veicolo* findMaximum(Veicolo* nodo){
    if(nodo == NULL)
        return NULL;
    while(nodo->dx != NULL){
        nodo = nodo->dx;
    }
    return nodo;
}

Stazione* findMin(Stazione* stazione){
    if (stazione == NULL)
        return NULL;

    while (stazione->sx != NULL) {
        stazione = stazione->sx;
    }

    return stazione;
}

Stazione* findMax(Stazione* stazione) {
    if (stazione == NULL)
        return NULL;

    while (stazione->dx != NULL) {
        stazione = stazione->dx;
    }

    return stazione;
}


// Funzione per il bilanciamento dell'albero dopo la cancellazione di un nodo
void fixDeletion(Veicolo** root, Veicolo* x) {
    while (x != *root && x->color == BLACK) {
        if (x == x->p->sx) {
            Veicolo* sibling = x->p->dx;

            // Caso 1: Il fratello di x è rosso
            if (sibling->color == RED) {
                sibling->color = BLACK;
                x->p->color = RED;
                leftRotate(root, x->p);
                sibling = x->p->dx;
            }

            // Caso 2: Il fratello di x è nero ed entrambi i suoi figli sono neri
            if ( sibling->sx->color == BLACK && sibling->dx->color == BLACK) {
                sibling->color = RED;
                x = x->p;
            } else {
                // Caso 3: Il fratello di x è nero e il figlio destro del fratello è nero
                if (sibling->dx->color == BLACK) {
                    sibling->sx->color = BLACK;
                    sibling->color = RED;
                    rightRotate(root, sibling);
                    sibling = x->p->dx;
                }

                // Caso 4: Il fratello di x è nero e il figlio destro del fratello è rosso
                sibling->color = x->p->color;
                x->p->color = BLACK;
                sibling->dx->color = BLACK;
                leftRotate(root, x->p);
                x = *root;
            }
        } else {
            // Stessi casi del blocco precedente, ma simmetrici per il lato destro
            Veicolo* sibling = x->p->sx;

            if (sibling->color == RED) {
                sibling->color = BLACK;
                x->p->color = RED;
                rightRotate(root, x->p);
                sibling = x->p->sx;
            }

            if (sibling->sx->color == BLACK && sibling->dx->color == BLACK) {
                sibling->color = RED;
                x = x->p;
            } else {
                if (sibling->sx->color == BLACK) {
                    sibling->dx->color = BLACK;
                    sibling->color = RED;
                    leftRotate(root, sibling);
                    sibling = x->p->sx;
                }

                sibling->color = x->p->color;
                x->p->color = BLACK;
                sibling->sx->color = BLACK;
                rightRotate(root, x->p);
                x = *root;
            }
        }
    }

    x->color = BLACK;
}

// Funzione per eliminare un veicolo con determinata autonomia (key)dall'albero rosso-nero (FONTE: PDF del cormen)
void deleteNode(Veicolo** root, int key) {
    Veicolo* nodeToDelete = findNode(*root, key);
    Veicolo* y = nodeToDelete;

    if (nodeToDelete == NULL) {
        printf("non rottamata\n");
        return;
    }

    Colore y_original_color = y->color;

    if (nodeToDelete->counter > 1){  // Caso: più veicoli con stessa autonomia
        nodeToDelete->counter--;
        printf("rottamata\n");
        return;
    }

    Veicolo* x;

    if (nodeToDelete->sx == NULL) {
        x = nodeToDelete->dx;
        rbTransplant(root, nodeToDelete, nodeToDelete->dx);
    } else if (nodeToDelete->dx == NULL) {
        x = nodeToDelete->sx;
        rbTransplant(root, nodeToDelete, nodeToDelete->sx);
    } else {
        y = findMinimum(nodeToDelete->dx);
        y_original_color = y->color;
        x = y->dx;

        if (y->p == nodeToDelete) {
            if (x != NULL) {
                x->p = y;
            }
        }else {
            rbTransplant(root, y, y->dx);
            y->dx = nodeToDelete->dx;
            y->dx->p = y;
        }

        rbTransplant(root, nodeToDelete, y);
        y->sx = nodeToDelete->sx;
        y->sx->p = y;
        y->color = nodeToDelete->color;
    }

    if (y_original_color == 'B')
        fixDeletion(root, x);


    free(nodeToDelete);
    printf("rottamata\n");

}


Stazione* findStazione(Stazione* root, int distanza) {
    if (root == NULL || root->distanza == distanza)
        return root;
    if (distanza < root->distanza)
        return findStazione(root->sx, distanza);
    return findStazione(root->dx, distanza);
}

// Funzione per aggiungere un veicolo alla stazione (solo per comodità)
void aggiungiVeicolo(Stazione* stazione,int autonomia) {
    rbInsert(&(stazione->veicoli), autonomia);
}

// (per comodità)
void rimuoviVeicolo(Stazione* stazione, int autonomia) {
    deleteNode(&(stazione->veicoli), autonomia);
}


Stazione* createStazione(int distanza) {
    Stazione* newStazione = (Stazione*)malloc(sizeof(Stazione));
    newStazione->distanza = distanza;
    newStazione->veicoli = NULL;
    newStazione->sx = NULL;
    newStazione->dx = NULL;
    newStazione->p = NULL;
    newStazione->color = RED; // inizializzo a rosso
    return newStazione;
}

void freeStazioneTree(Stazione* root) {
    if (root != NULL) {
        freeStazioneTree(root->sx);
        freeStazioneTree(root->dx);
        rbFree(&(root->veicoli)); // Libera l'albero dei veicoli di questa stazione
        free(root);
    }
}


void rimuoviStazione(Stazione** root, int distanza) {   /// SISTEMARE
    Stazione* stazione = findStazione(*root, distanza);
    if (stazione == NULL){
        printf("non demolita\n");
        return;
    }

    rbFree(&(stazione->veicoli)); // Libera l'albero dei veicoli di questa stazione

    if (stazione->sx == NULL || stazione->dx == NULL) {
        // La stazione da eliminare non ha entrambi i figli o ne ha almeno uno
        Stazione* child = (stazione->sx != NULL) ? stazione->sx : stazione->dx;
        if (child != NULL)
            child->p = stazione->p;
        if (stazione->p == NULL)
            *root = child;
        else {
            if (stazione == stazione->p->sx)
                stazione->p->sx = child;
            else
                stazione->p->dx = child;
        }
        free(stazione);
    } else {
        // La stazione da eliminare ha entrambi i figli, troviamo il successore
        Stazione* successor = findMin(stazione->dx);
        // Sostituisci il successore con la stazione da eliminare
        if (successor->p != stazione) {
            successor->p->sx = successor->dx;
            if (successor->dx != NULL)
                successor->dx->p = successor->p;
            successor->dx = stazione->dx;
            stazione->dx->p = successor;
        }
        successor->sx = stazione->sx;
        stazione->sx->p = successor;

        if (stazione == *root)
            *root = successor;

        free(stazione);

        rimuoviStazione(&(successor->dx), successor->distanza);
    }
    printf("demolita\n");
}


void rbDeleteFixupStazione(Stazione** tree, Stazione* x) {
    Stazione* w;

    while (x != *tree && x != NULL && x->color == BLACK) {
        if (x == x->p->sx) {
            w = x->p->dx;

            if (w != NULL && w->color == RED) {
                w->color = BLACK;
                x->p->color = RED;
                leftRotateStazione(tree, x->p);
                w = x->p->dx;
            }

            if ((w == NULL || (w->sx != NULL && w->sx->color == BLACK)) && (w == NULL || (w->dx != NULL && w->dx->color == BLACK))) {
                if (w != NULL) {
                    w->color = RED;
                }
                x = x->p;
            } else {
                if (w == NULL || w->dx == NULL || w->dx->color == BLACK) {
                    if (w->sx != NULL) {
                        w->sx->color = BLACK;
                    }
                    if (w != NULL) {
                        w->color = RED;
                    }
                    rightRotateStazione(tree, w);
                    w = x->p->dx;
                }

                if (x->p != NULL) {
                    w->color = x->p->color;
                    x->p->color = BLACK;
                }
                if (w->dx != NULL) {
                    w->dx->color = BLACK;
                }
                leftRotateStazione(tree, x->p);
                x = *tree;
            }
        } else {
            w = x->p->sx;

            if (w != NULL && w->color == RED) {
                w->color = BLACK;
                x->p->color = RED;
                rightRotateStazione(tree, x->p);
                w = x->p->sx;
            }

            if ((w == NULL || (w->dx != NULL && w->dx->color == BLACK)) && (w == NULL || (w->sx != NULL && w->sx->color == BLACK))) {

                if (w != NULL) {
                    w->color = RED;
                }
                x = x->p;
            } else {
                if (w == NULL || w->sx == NULL || w->sx->color == BLACK) {
                    if (w->dx != NULL) {
                        w->dx->color = BLACK;
                    }
                    if (w != NULL) {
                        w->color = RED;
                    }
                    leftRotateStazione(tree, w);
                    w = x->p->sx;
                }

                if (x->p != NULL) {
                    w->color = x->p->color;
                    x->p->color = BLACK;
                }
                if (w->sx != NULL) {
                    w->sx->color = BLACK;
                }
                rightRotateStazione(tree, x->p);
                x = *tree;
            }
        }
    }
    if (x != NULL) {
        x->color = BLACK;
    }
}

void freeStazioneNode(Stazione* stazione) {
    if (stazione != NULL) {
        rbFree(&(stazione->veicoli));
        free(stazione); // Libera il nodo stesso
    }
}

void rbDeleteStazione(Stazione** tree, Stazione* nodeToDelete) {
    Stazione* y = nodeToDelete;
    Colore y_original_color = y->color;

    Stazione* x;

    if (nodeToDelete->sx == NULL) {
        x = nodeToDelete->dx;
        rbTransplantStazione(tree, nodeToDelete, nodeToDelete->dx);
    } else if (nodeToDelete->dx == NULL) {
        x = nodeToDelete->sx;
        rbTransplantStazione(tree, nodeToDelete, nodeToDelete->sx);
    } else {
        y = findMin(nodeToDelete->dx);
        if (y == NULL) {
            // non esiste
            return;
        }
        y_original_color = y->color;
        x = y->dx;

        if (y->p == nodeToDelete) {
            if (x != NULL) {
                x->p = y;
            }
        }else {
            rbTransplantStazione(tree, y, y->dx);
            y->dx = nodeToDelete->dx;
            if (y->dx != NULL)
                y->dx->p = y;
        }

        rbTransplantStazione(tree, nodeToDelete, y);
        y->sx = nodeToDelete->sx;
        if (y->sx != NULL)
            y->sx->p = y;
        y->color = nodeToDelete->color;
    }

    if (x != NULL && y_original_color == RED)
        rbDeleteFixupStazione(tree, x);


    freeStazioneNode(nodeToDelete);
    printf("demolita\n");
}


// Funzione per creare un nuovo nodo della lista con la stazione specificata
ListNode* createN(Stazione *stazione) {
    ListNode* newNode = (ListNode*)malloc(sizeof(ListNode));
    newNode->stazione = stazione;
    newNode->next = NULL;
    return newNode;
}

// Funzione per l'inserimento di un elemento in testa alla lista
void insertAtHead(DoublyLinkedList* list, Stazione * stazione) {
    ListNode* newNode = createN(stazione);
    if (list->head == NULL) {
        list->head = list->tail = newNode;
    } else {
        newNode->next = list->head;
        list->head = newNode;
    }
    list->size++;
}

// Funzione per l'inserimento di un elemento in coda alla lista
void insertAtTail(DoublyLinkedList* list, Stazione* stazione) {
    ListNode* newNode = createN(stazione);
    if (list->tail == NULL) {
        list->head = list->tail = newNode;
    } else {
        list->tail->next = newNode;
        list->tail = newNode;
    }
    list->size++;
}

// Funzione per la rimozione di un elemento dalla testa della lista
void removeFromHead(DoublyLinkedList* list) {
    if (list->head == NULL) {
        return;
    }

    ListNode* nodeToRemove = list->head;
    list->head = list->head->next;
    if (list->head == NULL) {
        list->tail = NULL;
    }
    free(nodeToRemove);
    list->size--;
}


// Funzione per liberare la memoria allocata dalla lista
void freeList(DoublyLinkedList* list) {
    ListNode* current = list->head;
    while (current != NULL) {
        ListNode* next = current->next;
        free(current);
        current = next;
    }
    list->head = list->tail = NULL;
    list->size = 0;
}


Stazione* trovaSuccessore(Stazione* root, Stazione* node) {
    if (node->dx != NULL) {
        return findMin(node->dx);
    }

    Stazione* successor = NULL;
    while (root != NULL) {
        if (node->distanza < root->distanza) {
            successor = root;
            root = root->sx;
        } else if (node->distanza > root->distanza) {
            root = root->dx;
        } else {
            break;
        }
    }
    return successor;
}


Stazione* trovaPrecedente(Stazione* root, Stazione* node) {
    if (node->sx != NULL) {
        return findMax(node->sx);
    }

    Stazione* predecessor = NULL;
    while (root != NULL) {
        if (node->distanza > root->distanza) {
            predecessor = root;
            root = root->dx;
        } else if (node->distanza < root->distanza) {
            root = root->sx;
        } else {
            break;
        }
    }
    return predecessor;
}

struct Node* createNodeLinkedList(int data, struct Node* parent) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->data = data;
    newNode->next = NULL;
    newNode->parent = parent;
    return newNode;
}

struct LinkedList* createLinkedList() {
    struct LinkedList* list = (struct LinkedList*)malloc(sizeof(struct LinkedList));
    list->head = NULL;
    return list;
}


void appendNode(struct LinkedList* list, int data, struct Node* parent) {
    struct Node* newNode = createNodeLinkedList(data, parent);
    if (list->head == NULL) {
        list->head = newNode;
    } else {
        struct Node* current = list->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}

void appendIntoHead(struct LinkedList* list, int data, struct Node* parent) {
    struct Node* newNode = createNodeLinkedList(data, parent);
    if (list->head == NULL) {
        list->head = newNode;
        newNode->next = NULL;
    } else {
        newNode->next = list->head;
        list->head = newNode;
    }
}

struct Graph* createGraph(int numLists) {
    struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));
    graph->numLists = numLists;
    graph->listArray = (struct LinkedList**)malloc(numLists * sizeof(struct LinkedList*));
    for (int i = 0; i < numLists; i++) {
        graph->listArray[i] = createLinkedList();
    }
    return graph;
}

// Funzione per aggiungere un nuovo nodo alla lista contenuta ad un determinato livello di profondità
void addNodeToListInArray(struct Graph* graph, int listIndex, int data, struct Node* parent) {
    if (listIndex >= 0 && listIndex < graph->numLists) {
        appendNode(graph->listArray[listIndex], data, parent);
    } else {
        int newNumLists = listIndex + 1;
        struct LinkedList** newListArray = (struct LinkedList**)realloc(graph->listArray, newNumLists * sizeof(struct LinkedList*));
        if (newListArray == NULL) {
            //printf("Errore nell'allocazione di memoria.\n");
            return;
        }

        for (int i = graph->numLists; i < newNumLists; i++) {
            newListArray[i] = createLinkedList();
        }

        graph->listArray = newListArray;
        graph->numLists = newNumLists;

        appendNode(graph->listArray[listIndex], data, parent);
    }
}

//aggiungi nodo in testa alla lista
void addNodeToListHead(struct Graph* graph, int listIndex, int data, struct Node* parent) {
    if (listIndex >= 0 && listIndex < graph->numLists) {
        appendIntoHead(graph->listArray[listIndex], data, parent);
    } else {
        int newNumLists = listIndex + 1;
        struct LinkedList** newListArray = (struct LinkedList**)realloc(graph->listArray, newNumLists * sizeof(struct LinkedList*));
        if (newListArray == NULL) {
            //printf("Errore nell'allocazione di memoria.\n");
            return;
        }

        for (int i = graph->numLists; i < newNumLists; i++) {
            newListArray[i] = createLinkedList();
        }

        graph->listArray = newListArray;
        graph->numLists = newNumLists;

        appendIntoHead(graph->listArray[listIndex], data, parent);
    }
}


// Funzione per liberare la memoria occupata dall'albero generalizzato
void freeGraph(struct Graph* graph) {
    for (int i = 0; i < graph->numLists; i++) {
        struct Node* current = graph->listArray[i]->head;
        while (current != NULL) {
            struct Node* temp = current;
            current = current->next;
            free(temp);
        }
        free(graph->listArray[i]);
    }
    free(graph->listArray);
    free(graph);
}

// Funzione per stampare tutti i nodi in ordine dalla radice al nodo foglia di partenza
void printNodesFromRootToNode(struct Node* node) {
    if (node == NULL) {
        return;
    }
    printNodesFromRootToNode(node->parent);

    printf("%d ", node->data);
}

void cercaPercorso(Stazione * root, Stazione* a, Stazione* b){

    DoublyLinkedList lista1;
    lista1.head = lista1.tail = NULL;
    lista1.size = 0;

    DoublyLinkedList lista2;
    lista2.head = lista2.tail = NULL;
    lista2.size =0;

    Veicolo * v_max = NULL;

    //Vincolo 1: no auto disp dalla partenza -> no percorso
    if(a->veicoli == NULL){
        freeList(&lista1);
        freeList(&lista2);
        free(v_max);
        printf("nessun percorso\n");
        return;
    }

    // Caso di collegamento diretto
    v_max = findMaximum(a->veicoli);
    if(v_max->autonomia >= abs(b->distanza - a->distanza)){
        printf("%d %d\n", a->distanza, b->distanza);
        return;
    }

    struct Graph* graph = createGraph(1);  // albero generalizzato particolare
    struct Node* parent = NULL;

    addNodeToListInArray(graph, 0, a->distanza, parent);
    parent = graph->listArray[0]->head;

    insertAtHead(&lista1, a);
    int distMax = 0;
    Stazione* curr = a;
    Stazione* succ = NULL;
    int iterazione = 1; // indicherà anche il livello di profondità del mio albero generalizzato

    // CASO RITORNO
    if(a->distanza > b->distanza){
        distMax = a->distanza - v_max->autonomia;
        succ = trovaPrecedente(root,curr);

        // la destinazione è stazione successiva ma non ho stazioni intermedie e non ho collegamento diretto
        if( succ->distanza == b->distanza) {
            freeList(&lista1);
            //freeList(&lista2);
            freeGraph(graph);
            printf("nessun percorso\n");
            return;
        }

        while(distMax> b->distanza) {
            while (succ != NULL && distMax <= succ->distanza) {

                if (iterazione % 2 == 1) {
                    insertAtHead(&lista2, succ);
                    addNodeToListHead(graph, iterazione, succ->distanza, parent);

                } else{
                    insertAtHead(&lista1, succ);
                    addNodeToListHead(graph, iterazione, succ->distanza, parent);
                }
                succ = trovaPrecedente(root,succ);
            }

            if(iterazione%2 ==0)
                removeFromHead(&lista2);
            else
                removeFromHead(&lista1);

            // Siamo a 2,4,6,8.... archi di profondità
            if(iterazione%2 ==0){
                if(lista2.size ==0){
                    if(lista1.size > 0){
                        curr = lista1.head->stazione;
                        succ = trovaPrecedente(root,lista1.head->stazione);
                    }else{
                        printf("nessun percorso\n");
                        //freeList(&lista1); è gia vuota
                        freeList(&lista2);
                        freeGraph(graph);
                        return;
                    }
                    parent = graph->listArray[iterazione]->head;
                    iterazione++;

                }else {
                    curr = lista2.head->stazione;
                    parent = parent->next;

                    if( lista2.size == 1){
                        if(lista1.head != NULL){
                            succ = trovaPrecedente(root,lista1.head->stazione);
                        }else{
                            succ = trovaPrecedente(root,curr);
                        }
                    }else{
                        if(lista1.size >0){
                            if(lista1.head->stazione->distanza < lista2.head->stazione->distanza)
                                succ = trovaPrecedente(root,lista1.head->stazione);
                            else
                                succ = trovaPrecedente(root, lista2.head->stazione);
                        }
                    }
                }

            } else{ // Siamo a 1,3,5,7... archi di profondità
                if(lista1.size ==0) {  // Esaminato tutti i nodi della lista, passo all'altra
                    if (lista2.size > 0) {
                        curr = lista2.head->stazione;
                        succ = trovaPrecedente(root, lista2.head->stazione);
                    } else { // nessun nodo ha portato a nulla
                        printf("nessun percorso\n");
                        freeList(&lista1);
                        //freeList(&lista2); è già vuota
                        freeGraph(graph);
                        return;
                    }

                    parent = graph->listArray[iterazione]->head;
                    iterazione++;

                }else {  // Ho ancora dei nodi della lista da espandere
                    curr = lista1.head->stazione;
                    parent = parent->next;

                    if( lista1.size == 1){ // è l'ultimo nodo da espandere?
                        if(lista2.head != NULL){
                            succ = trovaPrecedente(root, lista2.head->stazione); // evita nodi comuni
                        }else{
                            succ = trovaPrecedente(root, curr);  //espandi senza vincoli
                        }
                    }else{ // ci sono altri nodi da espandere
                        if(lista2.size >0){
                            if(lista2.head->stazione->distanza < lista1.head->stazione->distanza)
                                succ = trovaPrecedente(root, lista2.head->stazione);
                            else
                                succ = trovaPrecedente(root, lista1.head->stazione);
                        }
                    }
                }
            }
            if(curr->veicoli != NULL) {
                distMax = curr->distanza - findMaximum(curr->veicoli)->autonomia;
            }else {
                distMax = curr->distanza;
            }
        }

        printNodesFromRootToNode(parent);
        printf("%d\n", b->distanza);

        freeList(&lista1);
        freeList(&lista2);
        freeGraph(graph);
        graph = NULL;
        v_max = NULL;
        curr = NULL;
        succ = NULL;
        a = NULL;
        b = NULL;
        parent = NULL;
        return;
    }

    // CASO ANDATA (cambia poco la implementazione, la logica è la stessa)
    distMax = a->distanza + v_max->autonomia;
    succ = trovaSuccessore(root, curr);


    // la destinazione è stazione successiva ma non ho stazioni intermedie e non ho collegamento diretto
    if( succ->distanza == b->distanza) {
        printf("nessun percorso\n");
        freeList(&lista1);
        freeList(&lista2);
        freeGraph(graph);
        return;
    }

    while(distMax< b->distanza) {

        // Passo I: ottengo tutte le stazioni a partire dal nodo di partenza
        while (distMax >= succ->distanza) {

            if (iterazione % 2 == 1) {
                insertAtTail(&lista2, succ);
                addNodeToListInArray(graph, iterazione, succ->distanza, parent);

            } else{
                insertAtTail(&lista1, succ);
                addNodeToListInArray(graph, iterazione, succ->distanza, parent);
            }
            succ = trovaSuccessore(root, succ);
        }
        if(iterazione%2 ==0)
            removeFromHead(&lista2);
        else
            removeFromHead(&lista1);

        // Siamo a 2,4,6,8.... archi di profondità
        if(iterazione%2 ==0){
            if(lista2.size ==0){
                if(lista1.size > 0){
                    curr = lista1.head->stazione;
                    succ = trovaSuccessore(root, lista1.tail->stazione);
                }else{
                    printf("nessun percorso\n");
                    freeList(&lista1);
                    freeList(&lista2);
                    freeGraph(graph);
                    return;
                }

                parent = graph->listArray[iterazione]->head;
                iterazione++;

            }else {
                curr = lista2.head->stazione;
                parent = parent->next;


                if( lista2.size == 1){
                    if(lista1.tail != NULL){
                        succ = trovaSuccessore(root, lista1.tail->stazione);
                    }else{
                        succ = trovaSuccessore(root, curr);
                    }
                }else{
                    if(lista1.size >0){
                        if(lista1.tail->stazione->distanza > lista2.tail->stazione->distanza)
                            succ = trovaSuccessore(root, lista1.tail->stazione);
                        else
                            succ = trovaSuccessore(root, lista2.tail->stazione);
                    }
                }
            }

        } else{ // Siamo a 1,3,5,7... archi di profondità
            if(lista1.size ==0) {  // Esaminato tutti i nodi della lista, passo all'altra
                if (lista2.size > 0) {
                    curr = lista2.head->stazione;
                    succ = trovaSuccessore(root, lista2.tail->stazione);
                } else { // nessun nodo ha portato a nulla
                    printf("nessun percorso\n");
                    freeList(&lista1);
                    freeList(&lista2);
                    freeGraph(graph);
                    return;
                }

                parent = graph->listArray[iterazione]->head; //scorri l'albero generalizzato di un livello
                iterazione++;

            }else {  // Ho ancora dei nodi della lista da espandere
                curr = lista1.head->stazione;
                parent = parent->next;

                if( lista1.size == 1){ // è l'ultimo nodo da espandere?
                    if(lista2.tail != NULL){
                        succ = trovaSuccessore(root, lista2.tail->stazione); // evita nodi comuni
                    }else{
                        succ = trovaSuccessore(root, curr);  //espandi senza vincoli
                    }
                }else{ // ci sono altri nodi da espandere
                    if(lista2.size >0){
                        if(lista2.tail->stazione->distanza > lista1.tail->stazione->distanza)
                            succ = trovaSuccessore(root, lista2.tail->stazione);
                        else
                            succ = trovaSuccessore(root, lista1.tail->stazione);
                    }
                }
            }
        }
        if(curr->veicoli != NULL) {
            distMax = curr->distanza + findMaximum(curr->veicoli)->autonomia;
        }else
            distMax = curr->distanza;
    }

    printNodesFromRootToNode(parent);
    printf("%d\n", b->distanza);

    freeList(&lista1);
    freeList(&lista2);
    freeGraph(graph);
    v_max = NULL;
    curr = NULL;
    succ = NULL;

    return;

}

int main(){

    char input[7000];
    while (fgets(input, sizeof(input), stdin) != NULL) {

        char command[20];

        if (sscanf(input, "%19s", command) != 1) {
            //printf("Comando non valido");
            continue;
        }

        if (strcmp(command, "aggiungi-stazione") == 0) {
            char* token = strtok(input, " ");
            int distance, num_vehicles;
            token = strtok(NULL, " ");
            distance = atoi(token);
            token = strtok(NULL, " ");
            num_vehicles = atoi(token);

            if(findStazione(autostrada,distance) != NULL){
                printf("non aggiunta\n");
            }else{
                Stazione* newStazione = createStazione(distance);

                for (int i = 0; i < num_vehicles; i++) {
                    token = strtok(NULL, " ");
                    aggiungiVeicolo(newStazione,atoi(token));
                }
                rbInsertStazione(&autostrada,newStazione);
                newStazione = NULL;

            }

        } else if (strcmp(command, "aggiungi-auto") == 0) {
            char* token = strtok(input, " ");
            int distance, autonomy;
            token = strtok(NULL, " ");
            distance = atoi(token);
            token = strtok(NULL, " ");
            autonomy = atoi(token);
            Stazione* x = findStazione(autostrada,distance);
            if( x != NULL) {
                aggiungiVeicolo(x, autonomy);
                printf("aggiunta\n");
            }else
                printf("non aggiunta\n");
        } else if (strcmp(command, "demolisci-stazione") == 0) {
            char* token = strtok(input, " ");
            int distance;
            token = strtok(NULL, " ");
            distance = atoi(token);
            Stazione* stazione = findStazione(autostrada, distance);
            if (stazione == NULL || autostrada == NULL){
                printf("non demolita\n");
            }else {
                rbDeleteStazione(&autostrada, stazione);
            }
            //free(stazione);
        } else if (strcmp(command, "rottama-auto") == 0) {
            char* token = strtok(input, " ");
            int distance, autonomy;
            token = strtok(NULL, " ");
            distance = atoi(token);
            token = strtok(NULL, " ");
            autonomy = atoi(token);
            Stazione* x = findStazione(autostrada, distance);
            if( x != NULL) {
                rimuoviVeicolo(x, autonomy);
                x = NULL;
            }else
                printf("non rottamata\n");

        } else if (strcmp(command, "pianifica-percorso") == 0) {
            char* token = strtok(input, " ");
            int start_distance, end_distance;
            token = strtok(NULL, " ");
            start_distance = atoi(token);
            token = strtok(NULL, " ");
            end_distance = atoi(token);
            Stazione * a = findStazione(autostrada, start_distance);
            Stazione * b = findStazione(autostrada, end_distance);
            if(a != NULL && b != NULL)
                cercaPercorso(autostrada, a, b);
                //printf("aspetta\n");
            else {
                printf("nessun percorso\n");
                free(a);
                free(b);
            }
        } else {
            //niente
        }
    }
    //freeStazioneTree(autostrada);

    return 0;
}
