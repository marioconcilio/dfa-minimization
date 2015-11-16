#include <stdio.h>
#include <stdlib.h>

#define true 1
#define false 0

typedef int bool;

typedef struct Node {
	int item;
	struct Node* next;
} Node;

typedef struct Queue{
	Node* head;
	Node* tail;
} Queue;

Queue newQueue() {
	Queue q;
	q.head = NULL;
	q.tail = NULL;
	return q;
}

bool isEmpty(Queue* q) {
	return (q->tail == NULL);
}

void display (Queue* q) {
    printf("\nDisplay: ");
    if (isEmpty(q)) {
        printf("Queue is empty.\n");
        return;
    }
   	Node* n = q->head;
    while(n) {
    	printf("%d ", n->item);
        n = n->next;
    }
    printf("\n\n");
}

void push(int item, Queue* q) {
	Node* n = (Node*) malloc(sizeof(Node));
	n->item = item;
	n->next = NULL;
	if (q->head == NULL) {
		q->head = n;
	} else {
		q->tail->next = n;
	}
	q->tail = n;
}

int pop(Queue *q) {
	Node* head = q->head;
	if (!head) return -1;
	int item = head->item;
	q->head = head->next;
	free(head);
	return item;
}

void imprimirDelta(int estados, int simbolos, int delta[estados][simbolos]) {
	printf("Transicoes: \n");
	int i; int j;
	for (i=0; i < estados; i++) {
		for (j=0; j < simbolos; j++) {
			printf("%d ", delta[i][j]);
		}
		printf("\n");
	}
}

void inicializarFinalizados(int estados, bool finalizados[estados]) {
	int i;
	for (i=0; i < estados; i++) {
		finalizados[i] = false;
	}
}

void imprimirFinalizados(int estados, bool finalizados[estados]) {
	printf("Estados finalizados: \n");
	int i;
	for (i = 0; i < estados; i++) {
		printf("%d ", finalizados[i]);
	}
	printf("\n");
}

int main(int argc, char const *argv[]) {
	FILE *file = fopen("afd-teste.txt", "r");
	if (!file) {
		printf("File not found!\n");
		exit(1);
	}

	int estados, simbolos, inicial;
	
	fscanf(file, "%d", &estados);
	printf("Quantidade de estados: %d\n", estados);

	fscanf(file, "%d", &simbolos);
	printf("Quantidade de simbolos: %d\n", simbolos);
	
	fscanf(file, "%d", &inicial);
	printf("Estado inicial: %d\n", inicial);

	bool aceitacao[estados]; int i;
	for (i = 0; i < estados; i++) {
		fscanf(file, "%d", &aceitacao[i]);
	}

	int delta[estados][simbolos]; int j;
	for (i = 0; i < estados; i++) {
		for (j=0; j < simbolos; j++) {
			fscanf(file, "%d", &delta[i][j]);
		}
	}

	imprimirDelta(estados, simbolos, delta);

	bool finalizados[estados];
	inicializarFinalizados(estados, finalizados);

	Queue acessiveis = newQueue();
	push(inicial, &acessiveis);

	while(!isEmpty(&acessiveis)) {
		int acessivel = pop(&acessiveis);
		if (acessivel == -1) break;
		for (i = 0; i < estados; i++) {
			int estado = delta[acessivel][i];
			if (estado != -1 && estado != acessivel && finalizados[estado] == false) {
				push(estado, &acessiveis);
			}
		}
		finalizados[acessivel] = true;		
	}

	imprimirFinalizados(estados, finalizados);

	fclose(file);
	return 0;
}