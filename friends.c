#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linked_list.h"
#include "friends.h"
#include "users.h"

#define DIE(assertion, call_description)            \
	do {                                            \
		if (assertion) {                            \
			fprintf(stderr, "(%s, %d): ", __FILE__, \
				__LINE__);                          \
			perror(call_description);               \
			exit(errno);                            \
		}                                           \
	} while (0)

#define MAX_QUEUE_SIZE 100

queue_t*
q_create(unsigned int data_size, unsigned int max_size)
{
	queue_t *q = calloc(1, sizeof(*q));
	DIE(!q, "calloc queue failed");
	q->data_size = data_size;
	q->max_size = max_size;
	q->buff = malloc(max_size * sizeof(*q->buff));
	DIE(!q->buff, "malloc buffer failed");
	return q;
}

unsigned int
q_get_size(queue_t *q)
{
	return !q ? 0 : q->size;
}

unsigned int
q_is_empty(queue_t *q)
{
	return !q ? 1 : !q->size;
}

void *q_front(queue_t *q)
{
	if (!q || !q->size)
		return NULL;

	return q->buff[q->read_idx];
}

int q_dequeue(queue_t *q)
{
	if (!q || !q->size)
		return 0;

	free(q->buff[q->read_idx]);

	q->read_idx = (q->read_idx + 1) % q->max_size;
	--q->size;
	return 1;
}

int q_enqueue(queue_t *q, void *new_data)
{
	void *data;
	if (!q || q->size == q->max_size)
		return 0;
	data = malloc(q->data_size);
	DIE(!data, "malloc data failed");
	memcpy(data, new_data, q->data_size);
	q->buff[q->write_idx] = data;
	q->write_idx = (q->write_idx + 1) % q->max_size;
	++q->size;
	return 1;
}

void q_clear(queue_t *q)
{
	unsigned int i;
	if (!q || !q->size)
		return;
	for (i = q->read_idx; i != q->write_idx; i = (i + 1) % q->max_size)
		free(q->buff[i]);
	q->read_idx = 0;
	q->write_idx = 0;
	q->size = 0;
}

void q_free(queue_t *q)
{
	if (!q)
		return;
	q_clear(q);
	free(q->buff);
	free(q);
}

static int is_node_in_graph(int n, int nodes)
{
	return n >= 0 && n < nodes;
}

list_graph_t*
lg_create(int nodes)
{
	int i;
	list_graph_t *g = malloc(sizeof(*g));
	DIE(!g, "malloc graph failed");
	g->neighbors = malloc(nodes * sizeof(*g->neighbors));
	DIE(!g->neighbors, "malloc neighbours failed");
	for (i = 0; i != nodes; ++i)
		g->neighbors[i] = ll_create(sizeof(int));
	g->nodes = nodes;
	return g;
}

void lg_add_edge(list_graph_t *graph, int src, int dest)
{
	if (!graph || !graph->neighbors || !is_node_in_graph(src, graph->nodes) ||
		!is_node_in_graph(dest, graph->nodes))
		return;

	ll_add_nth_node(graph->neighbors[src], graph->neighbors[src]->size, &dest);
}

static ll_node_t *find_node(linked_list_t *ll, int node, unsigned int *pos)
{
	ll_node_t *crt = ll->head;
	unsigned int i;

	for (i = 0; i != ll->size; ++i) {
		if (node == *(int *)crt->data) {
			*pos = i;
			return crt;
		}
		crt = crt->next;
	}

	return NULL;
}

int lg_has_edge(list_graph_t *graph, int src, int dest)
{
	unsigned int pos;

	if (!graph || !graph->neighbors || !is_node_in_graph(src, graph->nodes) ||
		!is_node_in_graph(dest, graph->nodes))
		return 0;

	return find_node(graph->neighbors[src], dest, &pos) ? 1 : 0;
}

linked_list_t*
lg_get_neighbours(list_graph_t *graph, int node)
{
	if (!graph || !graph->neighbors || !is_node_in_graph(node, graph->nodes))
		return NULL;

	return graph->neighbors[node];
}

void lg_remove_edge(list_graph_t *graph, int src, int dest)
{
	unsigned int pos;

	if (!graph || !graph->neighbors || !is_node_in_graph(src, graph->nodes) ||
		!is_node_in_graph(dest, graph->nodes))
		return;

	if (!find_node(graph->neighbors[src], dest, &pos))
		return;

	ll_node_t *removed = ll_remove_nth_node(graph->neighbors[src], pos);
	if (removed) {
		free(removed->data);
		free(removed);
	}
}

void lg_free(list_graph_t *graph)
{
	int i;

	for (i = 0; i != graph->nodes; ++i)
		ll_free(graph->neighbors + i);

	free(graph->neighbors);
	free(graph);
}

void add_friendship(list_graph_t *graph, char *user1, char *user2)
{
	int id1 = get_user_id(user1);
	int id2 = get_user_id(user2);

	lg_add_edge(graph, id1, id2);
	lg_add_edge(graph, id2, id1);
	printf("Added connection %s - %s\n", user1, user2);
}

void remove_friendship(list_graph_t *graph, char *user1, char *user2)
{
	int id1 = get_user_id(user1);
	int id2 = get_user_id(user2);

	lg_remove_edge(graph, id1, id2);
	lg_remove_edge(graph, id2, id1);
	printf("Removed connection %s - %s\n", user1, user2);
}

void print_min_path(list_graph_t *graph, char *user1, char *user2)
{
	int src = get_user_id(user1);
	int dest = get_user_id(user2);

	queue_t *q = q_create(sizeof(int), graph->nodes);
	int *prev = malloc(graph->nodes * sizeof(int));
	int *visited = calloc(graph->nodes, sizeof(int));
	int *distance = calloc(graph->nodes, sizeof(int));
	int current, i;

	for (i = 0; i < graph->nodes; i++) {
		prev[i] = -1;
		distance[i] = -1;
	}

	// start BFS
	visited[src] = 1;
	distance[src] = 0;
	q_enqueue(q, &src);

	while (!q_is_empty(q)) {
		current = *(int *)q_front(q);
		q_dequeue(q);

		linked_list_t *neighbors = lg_get_neighbours(graph, current);
		ll_node_t *neighbor = neighbors->head;
		while (neighbor) {
			int adj_node = *(int *)neighbor->data;
			if (!visited[adj_node]) {
				visited[adj_node] = 1;
				prev[adj_node] = current;
				distance[adj_node] = distance[current] + 1;
				q_enqueue(q, &adj_node);

				if (adj_node == dest) {
					printf("The distance between %s - %s is %d\n",
						   user1, user2, distance[dest]);
					q_free(q);
					free(prev);
					free(visited);
					free(distance);
					return;
				}
			}
			neighbor = neighbor->next;
		}
	}
	printf("There is no way to get from %s to %s\n", user1, user2);

	q_free(q);
	free(prev);
	free(visited);
	free(distance);
}

void suggestions(list_graph_t *graph, char *name)
{
	int user_id = get_user_id(name);

	int *suggestions = calloc(MAX_PEOPLE, sizeof(int));
	int *is_friend = calloc(MAX_PEOPLE, sizeof(int));

	linked_list_t *friends = lg_get_neighbours(graph, user_id);
	ll_node_t *friend_node = friends->head;

	// Mark direct friends
	while (friend_node) {
		int friend_id = *(int *)friend_node->data;
		is_friend[friend_id] = 1;
		friend_node = friend_node->next;
	}

	// Find friends of friends
	friend_node = friends->head;
	while (friend_node) {
		int friend_id = *(int *)friend_node->data;
		linked_list_t *friends_of_friend = lg_get_neighbours(graph, friend_id);
		ll_node_t *fof_node = friends_of_friend->head;

		while (fof_node) {
			int fof_id = *(int *)fof_node->data;
			if (fof_id != user_id && !is_friend[fof_id])
				suggestions[fof_id] = 1;
			fof_node = fof_node->next;
		}
		friend_node = friend_node->next;
	}

	// Print suggestions
	int found_suggestions = 0;
	for (int i = 0; i < MAX_PEOPLE; i++) {
		if (suggestions[i]) {
			if (!found_suggestions) {
				printf("Suggestions for %s:\n", name);
				found_suggestions = 1;
			}
			printf("%s\n", get_user_name(i));
		}
	}

	if (!found_suggestions)
		printf("There are no suggestions for %s\n", name);

	free(suggestions);
	free(is_friend);
}

int compare_ints(const void *a, const void *b) {
	int arg1 = *(const int *)a;
	int arg2 = *(const int *)b;
	return (arg1 > arg2) - (arg1 < arg2);
}

void print_common_friends(list_graph_t *graph, char *name1, char *name2) {
	int id1 = get_user_id(name1);
	int id2 = get_user_id(name2);

	linked_list_t *friends1 = lg_get_neighbours(graph, id1);
	linked_list_t *friends2 = lg_get_neighbours(graph, id2);

	if (!friends1 || !friends2) {
		printf("No common friends for %s and %s\n", name1, name2);
		return;
	}

	int *common_friends = calloc(MAX_PEOPLE, sizeof(int));
	int *ids = calloc(MAX_PEOPLE, sizeof(int));
	int count = 0;

	// Mark friends of first user
	for (ll_node_t *node1 = friends1->head; node1; node1 = node1->next) {
		int friend_id = *(int *)node1->data;
		common_friends[friend_id] = 1;
	}

	for (ll_node_t *node2 = friends2->head; node2; node2 = node2->next) {
		int friend_id = *(int *)node2->data;
		if (common_friends[friend_id])
			ids[count++] = friend_id;
	}

	// Sort the array of IDs and print the names in sorted order
	if (count > 0) {
		qsort(ids, count, sizeof(int), compare_ints);
		printf("The common friends between %s and %s are:\n", name1, name2);
		for (int i = 0; i < count; i++)
			printf("%s\n", get_user_name(ids[i]));
	} else {
		printf("No common friends for %s and %s\n", name1, name2);
	}

	free(common_friends);
	free(ids);
}

void print_friends_count(list_graph_t *graph, char *name)
{
	int user_id = get_user_id(name);

	linked_list_t *friends = lg_get_neighbours(graph, user_id);
	if (!friends)
		printf("%s has 0 friends\n", name);
	else
		printf("%s has %u friends\n", name, ll_get_size(friends));
}

void print_most_popular_friend(list_graph_t *graph, char *name)
{
	int user_id = get_user_id(name);

	linked_list_t *friends = lg_get_neighbours(graph, user_id);

	int max_id = user_id;
	int max_connections = ll_get_size(lg_get_neighbours(graph,
											   user_id));
	int friend_connections;
	ll_node_t *node = friends->head;

	while (node) {
		int friend_id = *(int *)node->data;
		friend_connections = ll_get_size(lg_get_neighbours(graph, friend_id));

		if (friend_connections > max_connections ||
			(friend_connections == max_connections && friend_id < max_id &&
			friend_connections > user_id)) {
			max_id = friend_id;
			max_connections = friend_connections;
		}
		node = node->next;
	}

	if (max_id == user_id) {
		printf("%s is the most popular\n", get_user_name(user_id));
	} else {
		printf("%s is the most popular friend of %s\n",
			   get_user_name(max_id), name);
	}
}

void handle_input_friends(char *input, list_graph_t *graph)
{
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");

	if (!cmd)
		return;

	if (!strcmp(cmd, "add")) {
		char *name1 = strtok(NULL, " ");
		char *name2 = strtok(NULL, "\n");
		add_friendship(graph, name1, name2);
	} else if (!strcmp(cmd, "remove")) {
		char *name1 = strtok(NULL, " ");
		char *name2 = strtok(NULL, "\n");
		remove_friendship(graph, name1, name2);
	} else if (!strcmp(cmd, "suggestions")) {
		char *name = strtok(NULL, "\n");
		suggestions(graph, name);
	} else if (!strcmp(cmd, "distance")) {
		char *name1 = strtok(NULL, " ");
		char *name2 = strtok(NULL, "\n");
		print_min_path(graph, name1, name2);
	} else if (!strcmp(cmd, "common")) {
		char *name1 = strtok(NULL, " ");
		char *name2 = strtok(NULL, "\n");
		print_common_friends(graph, name1, name2);
	} else if (!strcmp(cmd, "friends")) {
		char *name = strtok(NULL, "\n");
		print_friends_count(graph, name);
	} else if (!strcmp(cmd, "popular")) {
		char *name = strtok(NULL, "\n");
		print_most_popular_friend(graph, name);
	}
	free(commands);
}
