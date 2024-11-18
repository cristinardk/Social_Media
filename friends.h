#ifndef FRIENDS_H
#define FRIENDS_H

#define MAX_COMMAND_LEN 280
#define MAX_PEOPLE 550

#include "linked_list.h"

typedef struct list_graph {
	linked_list_t **neighbors;
	int nodes;
} list_graph_t;

// Queue structure
typedef struct queue {
	void **buff;              // Buffer holding the elements
	unsigned int data_size;   // Size of each element
	unsigned int max_size;    // Maximum number of elements
	unsigned int size;        // Current number of elements
	unsigned int read_idx;    // Index for reading
	unsigned int write_idx;   // Index for writing
} queue_t;

// Function declarations

// Graph functions
list_graph_t *lg_create(int nodes);
void handle_input_friends(char *input, list_graph_t *graph);
void lg_free(list_graph_t *graph);
linked_list_t *lg_get_neighbours(list_graph_t *graph, int node);
int lg_has_edge(list_graph_t *graph, int src, int dest);
int compare_ints(const void *a, const void *b);
#endif // FRIENDS_H
