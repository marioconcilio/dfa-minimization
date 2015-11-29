#include <stdio.h>
#include <stdlib.h>

#define true 1
#define false 0

typedef int bool;

typedef struct Node {
	int 	state;
	int 	symbol;
	struct 	Node* next;
} Node;

typedef struct Graph {
	Node* 	start;
	int 	states;
	bool	deleted;
	bool	initial;
	bool	acceptance;
	bool 	visited;
	bool 	finalized;
} Graph;

Node* newNode() {
	Node* new = (Node*) malloc(sizeof(Node));
	new->state = 0;
	new->symbol = 0;
	new->next = NULL;
	return new;
}

void initGraph(Graph* g) {
	int i;
	for (i = 0; i < g->states; i++) {
		g[i].start = NULL;
		g[i].initial = false;
		g[i].deleted = false;
		g[i].acceptance = false;
		g[i].visited = false;
		g[i].finalized = false;
	}
}

Graph* newGraph(int states) {
	Graph* g = (Graph*) malloc(sizeof(Graph) * states);
	g->states = states;
	initGraph(g);
	return g;
}

void displayGraph(Graph* g) {
	int i;
	for(i = 0; i < g->states; i++) {
		if(g[i].deleted) continue;
		printf("%d ", i);
		Node* n = g[i].start;
		while(n) {
			printf("--%d-> (%d) ", n->symbol, n->state);
			n = n->next;
		}
		printf("\n");
	}
	printf("\n");
}

bool edgeExists(Graph* g, int from, int to, int symbol) {
	Node* n = g[from].start;
	while(n) {
		if(n->state == to && n->symbol == symbol) {
			return true;
		}
		n = n->next;
	}
	return false;
}

bool addEdge(Graph* g, int from, int to, int symbol) {
	if (from < 0 || to < 0 || symbol < 0) return false;
	if (edgeExists(g, from, to, symbol)) return false;
	Node* new = newNode();
	new->state = to;
	new->symbol = symbol;
	new->next = g[from].start;
	g[from].start = new;
	return true;
}

bool removeEdge(Graph* g, int from, int to, int symbol) {
	Node* last = NULL; bool found = false;
	Node* n = g[from].start;
	while(n) {
		if(n->state == to && n->symbol == symbol) {
			found = true;
			break;
		}
		last = n;
		n = n->next;
	}
	if(!found) return false;
	if(last) last->next = n->next;
	else g[from].start = n->next;
	free(n);
	return true;
}

void removeState(Graph* g, int state) {
	int i;
	for (i = 0; i < g->states; i++) {
		Node* n = g[i].start;
		while(n) {
			if(n->state == state) {
				removeEdge(g, i, state, n->symbol);
			}
			n = n->next;
		}
	}
	g[state].deleted = true;

}

void initVisited(Graph* g) {
	int i;
	for (i=0; i < g->states; i++) {
		g[i].visited = false;
	}
}

void initFinalized(Graph* g) {
	int i;
	for (i = 0; i < g->states; i++) {
		g[i].finalized = false;
	}
}

void dfs(Graph* g, int state) {
	if (!g[state].deleted) {
		g[state].visited = true;
		Node* n = g[state].start;
		while(n) {
			if(!g[n->state].visited) dfs(g, n->state);
			n = n->next;
		}
		g[state].finalized = true;
	}
}

void inacessible(Graph* g, int initial) {
	dfs(g, initial);
	int i;
	for(i = 0; i < g->states; i++) {
		if(!g[i].finalized) removeState(g, i);
	}
	initVisited(g);
	initFinalized(g);
}

int main(int argc, char const *argv[]) {
	FILE *file = fopen("afd-teste.txt", "r");
	if (!file) {
		printf("File not found!\n");
		exit(1);
	}

	int states, symbols, initial;
	
	fscanf(file, "%d", &states);
	printf("Number of states: %d\n", states);

	fscanf(file, "%d", &symbols);
	printf("Numer of symbols: %d\n", symbols);
	
	fscanf(file, "%d", &initial);
	printf("Initial state: %d\n", initial);

	Graph* dfa = newGraph(states);

	// Set initial state.
	dfa[initial].initial = true;

	// Set acceptance states.
	int i;
	for (i = 0; i < states; i++) {
		fscanf(file, "%d", &dfa[i].acceptance);
	}

	// Insert transitions.
	int j; int to;
	for (i = 0; i < states; i++) {
		for (j=0; j < symbols; j++) {
			fscanf(file, "%d", &to);
			addEdge(dfa, i, to, j);
		}
	}

	displayGraph(dfa);

	// Remove inaccessible states
	inacessible(dfa, initial);

	displayGraph(dfa);

	fclose(file);
	return 0;
}