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
Graph* removeState(Graph* g, int state);
void initVisited(Graph* g);
void initFinalized(Graph* g);
void dfs(Graph* g, int state);
void unreachable(Graph** g, int initial);
Graph* reverse(Graph* g);
void useless(Graph** g);
Graph* tableFill(Graph* g, int symbols);
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
	Graph *g = (Graph*) malloc(sizeof(Graph) * states);
	g->states = states;
	initGraph(g);
	return g;
}

// Display a graph on screen.
void displayGraph(Graph* g) {
	int i;
	for(i = 0; i < g->states; i++) {
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

// Remove the given state on g graph. Remove a state,
// here, it means to delete all the edges that reach the given
// state and set the state 'deleted' variable = true.
Graph* removeState(Graph* g, int state) {
	printf("Removendo estado: %d\n", state);
	if(g[state].initial) return g;
	Graph* copy = newGraph(g->states - 1); int i;
	for (i = 0; i < g->states; i++) {
		if(i == state) continue;
		if(g[i].initial) {
			if(i < state) copy[i].initial = true;
			else copy[i-1].initial = true;
		}
		if(g[i].acceptance) {
			if(i < state) copy[i].acceptance = true;
			else copy[i-1].acceptance = true;
		}
		Node* n = g[i].start;
		while(n) {
			if(n->state != state) {
				if(i < state) {
					if(n->state < state) addEdge(copy, i, n->state, n->symbol);
					else addEdge(copy, i, n->state-1, n->symbol);
				} else {
					if(n->state < state) addEdge(copy, i-1, n->state, n->symbol);
					else addEdge(copy, i-1, n->state-1, n->symbol);
				}
			}
			if(i > state && n->state == state) {
				addEdge(copy, i-1, i-1, n->symbol);
			}
			n = n->next;
		}
	}
	printf("Removido:\n");
	displayGraph(copy);
	return copy;
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
// transitions. At the end, returns the new graph with the deleted
// states removed.
void unreachable(Graph** g, int initial) {
	Graph* gp = *g;
	dfs(gp, initial);
	int i;
	for(i = 0; i < gp->states; i++) {
		if(!gp[i].finalized) *g = removeState(gp, i);
	}
	initFinalized(*g);
	initVisited(*g);
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
	printf("Reverse:\n");
	displayGraph(r);
	return r;
}

// Removes useless states by reversing all the transitions
// in the automata, creating an aditional state that points
// to all acceptance states and using dfs to determine the states
// that does not reach the acceptation ones.
void useless(Graph** g) {
	Graph* r = reverse(*g);
	dfs(r, r->states-1); int i;
	for(i = 0; i < r->states; i++) {
		if(!r[i].finalized) *g = removeState(*g, i);
	}
	initFinalized(*g);
	initVisited(*g);
}

Graph* tableFill(Graph* g, int symbols) {
	bool table[g->states][g->states];
	int i; int j; int k;

	// Given a pair of states, if one is acceptance
	// and the other is not, they cannot be equivalent.
	// So, we mark the pair as false.
	for(i = 0; i < g->states; i++) {
		for(j = 0; j < g->states; j++) {
			if((g[i].acceptance && g[j].acceptance) || (!g[i].acceptance && !g[j].acceptance)) {
				table[i][j] = true;
			} else {
				table[i][j] = false;
			}
		}
	}

	for(i = 0; i < g->states; i++) {
		for(j = 0; j < g->states; j++) {
			if(table[i][j]) {
				for(k = 0; k < symbols; k++) {
					int q1 = search(g, i, k);
					int q2 = search(g, j, k);
					if((q1 == -1 && q2 != -1) || (q1 != -1 && q2 == -1)) {
						table[i][j] = false;
					}
				}
			}
		}
	}

	printf("Before:\n");
	for(i = 0; i < g->states; i++) {
		for(j = 0; j < g->states; j++) {
			printf("%d ", table[i][j]);
		}
		printf("\n");
	}

	// This loop executes until there is no change on table,
	// when the variable count remains 0.
	int count = 0;
	while(true) {
		count = 0;
		for(i = 0; i < g->states; i++) {
			for(j = 0; j < g->states; j++) {
				if(table[i][j]) {
					for(k = 0; k < symbols; k++) {
						int q1 = search(g, i, k);
						int q2 = search(g, j, k);
						if(q1 != -1 && q2 != -1 && !table[q1][q2]) {
							table[i][j] = false; count++;
						}
					}
				}
			}
		}
		if(count == 0) break;
	}
	
	printf("After:\n");
	for(i = 0; i < g->states; i++) {
		for(j = 0; j < g->states; j++) {
			printf("%d ", table[i][j]);
		}
		printf("\n");
	}

	int rep[g->states];
	for(i = 0; i < g->states; i++) {
		rep[i] = -1;
	}

	count = 0;
	while(true) {
		int times = 0;
		for(i = 0; i < g->states; i++) {
			if(rep[i] == -1) {
				count++;
				rep[i] = count -1;
				for(j = 0; j < g->states; j++) {
					if(table[i][j]) rep[j] = rep[i];
					times++;
				}
			}
		}
		if(times == 0) break;
	}

	printf("REQ: %d\n", count);
	for(i = 0; i < g->states; i++) {
		printf("%d ", rep[i]);
	}
	printf("\n");

	Graph* m = newGraph(count);
	for(i = 0; i < g->states; i++) {
		if(g[i].initial) m[rep[i]].initial = true;
		if(g[i].acceptance) m[rep[i]].acceptance = true;
		Node* n = g[i].start;
		while(n) {
			addEdge(m, rep[i], rep[n->state], n->symbol);
			n = n->next;
		}
	}

	printf("New GRAPH:\n");
	displayGraph(m);

	return m;
}

void writeDFA(Graph* g, int symbols, FILE* file) {
	int i; int initial = 0;
	for(i = 0; i < g->states; i++) {
		if(g[i].initial) initial = i;
	}
	fprintf(file, "%d ", g->states);
	fprintf(file, "%d ", symbols);
	fprintf(file, "%d\n", initial);
	
	for(i = 0; i < g->states; i++) {
		fprintf(file, "%d ", g[i].acceptance);
	}

	int j;
	for(i = 0; i < g->states; i++) {
		fprintf(file, "\n");
		for(j = 0; j < symbols; j++) {
			fprintf(file, "%d ", search(g, i, j));
		}
	}

	fprintf(file, "\n");
}

int main(int argc, char const *argv[]) {
	if(argc != 3) {
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
	for(i = 0; i < states; i++) {
		fscanf(input, "%d", &dfa[i].acceptance);
	}

	// Insert transitions.
	int j; int to;
	for(i = 0; i < states; i++) {
		for (j = 0; j < symbols; j++) {
			fscanf(input, "%d", &to);
			addEdge(dfa, i, to, j);
		}
	}

	printf("Original:\n");
	displayGraph(dfa);

	// Eliminate unreachable states.
	unreachable(&dfa, initial);

	printf("Unreachable:\n");
	displayGraph(dfa);

	// Eliminate useless states.
	useless(&dfa);

	printf("Useless states removed:\n");
	displayGraph(dfa);

	// Write the minimized DFA on the output file.
	writeDFA(tableFill(dfa, symbols), symbols, out);

	// Close files.
	fclose(input);
	fclose(out);

	return 0;
}