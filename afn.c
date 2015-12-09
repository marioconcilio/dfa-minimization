#include <stdio.h>
#include <stdlib.h>
#include "afd_util.h"
#define true 1
#define false 0

typedef int bool;

typedef struct Node {
	int 	state;
	int 	symbol;
	struct 	Node* next;
} Node;

// List of adjacencies graph implementation.
typedef struct Graph {
	Node* 	start;
	int 	states;
	bool	initial;
	bool	acceptance;
	bool 	visited;
	bool 	finalized;
} Graph;

Node* newNode();
void initGraph(Graph* g);
Graph* newGraph(int states);
void displayGraph(Graph* g);
bool edgeExists(Graph* g, int from, int to, int symbol);
bool addEdge(Graph* g, int from, int to, int symbol);
bool isDeleted(Graph* g, int state);
bool removeEdge(Graph* g, int from, int to, int symbol);
void removeState(Graph* g, int state);
void initVisited(Graph* g);
void initFinalized(Graph* g);
void dfs(Graph* g, int state);
void unreachable(Graph* g, int initial);
Graph* reverse(Graph* g);
void useless(Graph* g);

// Return a new allocated node.
Node* newNode(int state, int symbol, Node* next) {
	Node* new = (Node*) malloc(sizeof(Node));
	new->state = state;
	new->symbol = symbol;
	new->next = next;
	return new;
}

// Initializate a graph with its default value.
void initGraph(Graph* g) {
	int i;
	for (i = 0; i < g->states; i++) {
		g[i].start = NULL;
		g[i].initial = false;
		g[i].acceptance = false;
		g[i].visited = false;
		g[i].finalized = false;
	}
}

// Allocate memory , initialize and returns the new graph.
Graph* newGraph(int states) {
	Graph* g = (Graph*) malloc(sizeof(Graph) * states);
	g->states = states;
	initGraph(g);
	return g;
}

// Display a graph on screen.
// Do not show the deleted states.
void displayGraph(Graph* g) {
	int i;
	for(i = 0; i < g->states; i++) {
//		if(g[i].deleted) continue;
		printf("%d ", i);
		Node* n = g[i].start;
		while(n) {
			printf("--%d-> (%d) ", n->symbol, n->state);
			n = n->next;
		}
		if(g[i].initial) printf("INITIAL");
		if(g[i].acceptance) printf("ACCEPTANCE");
		printf("\n");
	}
	printf("\n");
}

// Search for an edge in graph, using 'from' as origin
// and 'to' as destiny, with 'symbol' as its weight.
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

// Insert a new edge on graph g, 'from' a node
// 'to' another by the given 'symbol'.
bool addEdge(Graph* g, int from, int to, int symbol) {
	if(from < 0 || to < 0 || symbol < 0) return false;
	if(edgeExists(g, from, to, symbol)) return false;
	Node* new = newNode(to, symbol, g[from].start);
	g[from].start = new;
	return true;
}

// Returns if a given state is deleted or not.
// A state is deleted if does not exists a node
// comming out of it.
bool isDeleted(Graph* g, int state) {
	return g[state].start == NULL;
}

// Removes an edge on g graph 'from' a node 'to'
// another by the given 'symbol'.
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

// Remove the given state on g graph. Remove a state,
// here, it means to delete all the edges that reach the given
// state and set the state 'deleted' variable = true.
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
	// Remove all edges starting on the deleted node.
	Node* n = g[state].start;
	while(n) {
		removeEdge(g, state, n->state, n->symbol);
		n = n->next;
	}
}

// Set visited = true on all nodes in g graph.
// The variable visited is used in some algorithms,
// for example, search for inaccessible states.
void initVisited(Graph* g) {
	int i;
	for (i=0; i < g->states; i++) {
		g[i].visited = false;
	}
}

// Set finalized = true on all nodes in g graph.
// The variable finalized is used in some algorithms,
// for example, search for inaccessible states.
void initFinalized(Graph* g) {
	int i;
	for (i = 0; i < g->states; i++) {
		g[i].finalized = false;
	}
}

// Depth-first search implementation. It marks all
// the reachable nodes, starting from the given 'state'.
// The ones that are not marked, can be seen as unreachable
// or inaccessible.
void dfs(Graph* g, int state) {
	g[state].visited = true;
	Node* n = g[state].start;
	while(n) {
		if(!g[n->state].visited) dfs(g, n->state);
		n = n->next;
	}
	g[state].finalized = true;
}

// Uses depth-first search to determine the unreachable states.
// For each one of them, it removes the state and the respective
// transitions.
void unreachable(Graph* g, int initial) {
	dfs(g, initial);
	int i;
	for(i = 0; i < g->states; i++) {
		if(!g[i].finalized) removeState(g, i);
	}
	initVisited(g);
	initFinalized(g);
}

// Return a graph with all edges reversed
// and with an aditional state pointing
// to all accepting states.
Graph* reverse(Graph* g) {
	Graph* r = newGraph(g->states + 1);
	int i;
	for(i = 0; i < g->states; i++) {
		r[i].initial = g[i].initial;
		r[i].acceptance = g[i].acceptance;
		Node* n = g[i].start;
		while(n) {
			addEdge(r, n->state, i, n->symbol);
			n = n->next;
		}
		if(g[i].acceptance) {
			addEdge(r, g->states, i, 0);
		}
	}
	return r;
}

// Removes useless states by reversing all the transitions
// in the automata and using dfs to determine the states
// that does not reach the acceptation ones.
void useless(Graph* g) {
	Graph* r = reverse(g);
	dfs(r, r->states-1);
	int i;
	for(i = 0; i < r->states-1; i++) {
		if(!r[i].finalized) removeState(g, i);
	}
}

void tableFill(Graph* g) {
	bool table[g->states][g->states];
	int i; int j;
	for(i = 0; i < g->states; i++) {
		for(j = 0; j < g->states; j++) {
			if(i == j) {
				table[i][j] = true;
				continue;
			}
			table[i][j] = false;
		}
	}
	for(i = 0; i < g->states; i++) {
		for(j = 0; j < i; j++) {

			if(g[i].acceptance ^ g[j].acceptance) {
				table[i][j] = true;
			}
		}
	}
	for(i = 0; i < g->states; i++) {
		for(j = 0; j < g->states; j++) {
			printf("%d ", table[i][j]);
		}
		printf("\n");
	}
}

int main(int argc, char const *argv[]) {
	FILE *file = fopen("examples/afd4.txt", "r");
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
		for (j = 0; j < symbols; j++) {
			fscanf(file, "%d", &to);
			addEdge(dfa, i, to, j);
		}
	}

	printf("Original:\n");
	displayGraph(dfa);

	unreachable(dfa, initial);
	
	printf("Unreachable states removed:\n");
	displayGraph(dfa);

	useless(dfa);
	
	printf("Useless states removed:\n");
	displayGraph(dfa);

	fclose(file);
	return 0;
}
