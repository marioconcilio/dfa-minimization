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

// List of adjacencies graph implementation.
typedef struct Graph {
	Node* 	start;
	int 	states;
	bool	deleted;
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
bool removeEdge(Graph* g, int from, int to, int symbol);
void removeState(Graph* g, int state);
void initVisited(Graph* g);
void initFinalized(Graph* g);
void dfs(Graph* g, int state);
void unreachable(Graph* g, int initial);

// Return a new allocated node.
Node* newNode() {
	Node* new = (Node*) malloc(sizeof(Node));
	new->state = 0;
	new->symbol = 0;
	new->next = NULL;
	return new;
}

// Initializate a graph with its default value.
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
	if (from < 0 || to < 0 || symbol < 0) return false;
	if (edgeExists(g, from, to, symbol)) return false;
	Node* new = newNode();
	new->state = to;
	new->symbol = symbol;
	new->next = g[from].start;
	g[from].start = new;
	return true;
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
	g[state].deleted = true;
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

// Uses depth-first search to determine the unreachable states.
// For each one of them, it removes the state and the respectives
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

	unreachable(dfa, initial);

	displayGraph(dfa);

	fclose(file);
	return 0;
}