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
int search(Graph* g, int from, int symbol);
bool addEdge(Graph* g, int from, int to, int symbol);
bool isDeleted(Graph* g, int state);
bool removeEdge(Graph* g, int from, int to, int symbol);
void removeAllEdges(Graph* g, int state);
void removeState(Graph* g, int state);
void initVisited(Graph* g);
void initFinalized(Graph* g);
void dfs(Graph* g, int state);
void unreachable(Graph* g, int initial);
Graph* reverse(Graph* g);
void useless(Graph* g);
void tableFill(Graph* g, int symbols);
void writeDFA(Graph* g, int symbols, FILE* file);

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

// Returns the next state 'from' the given state
// with the given 'symbol'.
int search(Graph* g, int from, int symbol) {
	Node* n = g[from].start;
	while(n) {
		if(n->symbol == symbol) return n->state;
		n = n->next;
	}
	return -1;
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

// Remove all edges from the given state.
void removeAllEdges(Graph* g, int state) {
	Node* n = g[state].start;
	while(n) {
		removeEdge(g, state, n->state, n->symbol);
		n = n->next;
	}
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
	removeAllEdges(g, state);
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
// in the automata, creating an aditional state that points
// to all acceptance states and using dfs to determine the states
// that does not reach the acceptation ones.
void useless(Graph* g) {
	Graph* r = reverse(g);
	dfs(r, r->states-1);
	int i;
	for(i = 0; i < r->states-1; i++) {
		if(!r[i].finalized) removeState(g, i);
	}
}

// Removes the equivalent states by using
// the table filling technique.
void tableFill(Graph* g, int symbols) {
	bool table[g->states][g->states];
	int i; int j; int k;

	// Table initialization.
	for(i = 0; i < g->states; i++) {
		for(j = 0; j < g->states; j++) {
			if (i == j) table[i][j] = true;
			else table[i][j] = false;
		}
	}

	// Given a pair of states, if one is acceptance
	// and the other is not, they cannot be equivalent.
	// So, we mark the pair as true.
	for(i = 0; i < g->states; i++) {
		for(j = 0; j < i; j++) {
			if(g[i].acceptance ^ g[j].acceptance) {
				table[i][j] = true;
			}
		}
	}

	// This loop executes until there is no change on table,
	// when the variable count remains 0.
	while(true) {
		int count = 0;
		for(i = 0; i < g->states; i++) {
			for(j = 0; j < i; j++) {
				for(k = 0; k < symbols; k++) {
					if(!table[i][j]) {
						int q1 = search(g, i, k);
						int q2 = search(g, j, k);
						if(q1 >= 0 && q2 >= 0 && !table[q1][q2]) {
							table[i][j] = true; count++;
						}
					}
				}
			}
		}
		if(count == 0) break;
	}

	for(i = 0; i < g->states; i++) {
		for(j = 0; j < g->states; j++) {
			printf("%d ", table[i][j]);
		}
		printf("\n");
	}

	// Remove the states that are not marked.
	for(i = 0; i < g->states; i++) {
		for(j = 0; j < i; j++) {
			if(!table[i][j]) removeState(g, i);
		}
	}
}

// Writes a dfa on the given file.
void writeDFA(Graph* g, int symbols, FILE* file) {
	int i; int j;
	int validStates = 0; int initial;
	for(i = 0; i < g->states; i++) {
		if(!isDeleted(g, i)) {
			validStates++;
			if(g[i].initial) initial = i;
		}
	}
	fprintf(file, "%d ", validStates);
	fprintf(file, "%d ", symbols);
	fprintf(file, "%d\n", initial);
	for(i = 0; i < g->states; i++) {
		if(!isDeleted(g, i)) fprintf(file, "%d ", g[i].acceptance);
	}
	for(i = 0; i < g->states; i++) {
		if(isDeleted(g, i)) continue;
		fprintf(file, "\n");
		for(j = 0; j < symbols; j++) {
			 fprintf(file, "%d ", search(g, i, j));
		}
	}
	fprintf(file, "\n");
}

int main(int argc, char const *argv[]) {
	if(argc != 3	) {
		printf("Usage:\n");
		printf("afn <input dfa> <minmized dfa>\n");
		exit(1);
	}

	FILE *input = fopen(argv[1], "r");
	if (!input) {
		printf("File not found!\n");
		exit(1);
	}

	FILE* out = fopen(argv[2], "w");
	if(!out) {
		printf("Invalid output file\n");
		exit(1);
	}

	int states, symbols, initial;
	
	fscanf(input, "%d", &states);
	fscanf(input, "%d", &symbols);
	fscanf(input, "%d", &initial);

	Graph* dfa = newGraph(states);

	// Set initial state.
	dfa[initial].initial = true;

	// Set acceptance states.
	int i;
	for (i = 0; i < states; i++) {
		fscanf(input, "%d", &dfa[i].acceptance);
	}

	// Insert transitions.
	int j; int to;
	for (i = 0; i < states; i++) {
		for (j = 0; j < symbols; j++) {
			fscanf(input, "%d", &to);
			addEdge(dfa, i, to, j);
		}
	}

	printf("Original:\n");
	displayGraph(dfa);

	// Eliminate unreachable states.
	unreachable(dfa, initial);

	// Eliminate useless states.
	useless(dfa);

	// Eliminate equivalent states.
	tableFill(dfa, symbols);

	printf("Equivalent states removed:\n");
	displayGraph(dfa);

	// Write the minimized DFA on the output file.
	writeDFA(dfa, symbols, out);

	// Close files.
	fclose(input);
	fclose(out);

	return 0;
}