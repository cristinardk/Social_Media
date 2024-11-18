#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "feed.h"
#include "users.h"
#include "friends.h"
#include "linked_list.h"
#include "posts.h"

void feed_command(list_graph_t *graph, char *user_name, int feed_size,
				  linked_list_t *lists_of_posts)
{
	int user_id = get_user_id(user_name);

	linked_list_t *friends = lg_get_neighbours(graph, user_id);
	post **feed_posts = (post **)malloc(feed_size * sizeof(post *));
	int feed_count = 0;

	// Iterate through the list of posts
	ll_node_t *node = lists_of_posts->head;
	while (node && feed_count < feed_size) {
		post *current_post = (post *)node->data;
		if (current_post->uid == user_id) {
			feed_posts[feed_count++] = current_post;
		} else {
			// Search in the user's friends list
			ll_node_t *friend_node = friends->head;
			while (friend_node) {
				if (current_post->uid == *(int *)friend_node->data) {
					feed_posts[feed_count++] = current_post;
					break;
				}
				friend_node = friend_node->next;
			}
		}
		node = node->next;
	}

	for (int i = feed_count - 1; i >= 0; i--) {
		printf("%s: \"%s\"\n", get_user_name(feed_posts[i]->uid),
			   feed_posts[i]->title);
	}

	 // Free allocated memory
	free(feed_posts);
}

void find_and_print_reposts_by_user(tree_node_t *root, int user_id) {
	post *post_data = (post *)root->data;
	if (post_data && post_data->uid == user_id)
		printf("Reposted: \"%s\"\n", post_data->title);
	for (int i = 0; i < root->n_children; i++)
		find_and_print_reposts_by_user(root->children[i], user_id);
}

void view_profile_command(char *user_name, linked_list_t *lists_of_posts) {
	int user_id = get_user_id(user_name);

	ll_node_t *node = lists_of_posts->head;
	post *current_post;

	// First while loop: find and display the user's posts
	while (node) {
		current_post = (post *)node->data;
		if (current_post->uid == user_id)
			printf("Posted: \"%s\"\n", current_post->title);
		node = node->next;
	}
	// Reset node to the beginning of the list to iterate again
	node = lists_of_posts->head;

	// Second while loop: find and display the user's reposts
	while (node) {
		current_post = (post *)node->data;
		if (current_post->events) {
			find_and_print_reposts_by_user(current_post->events->root,
										   user_id);
		}
		node = node->next;
	}
}

void find_and_print_friends_reposts(tree_node_t *root, int user_id,
									int friend_id)
{
	post *post_data = (post *)root->data;
	if (post_data && post_data->uid == friend_id)
		printf("%s\n", get_user_name(friend_id));
	for (int i = 0; i < root->n_children; i++)
		find_and_print_friends_reposts(root->children[i], user_id,
									   friend_id);
}

void friends_repost_command(list_graph_t *graph, char *user_name,
							int post_id, linked_list_t *lists_of_posts)
{
	int user_id = get_user_id(user_name);
	// Get the user's friends list
	linked_list_t *friends_list = lg_get_neighbours(graph, user_id);

	// Find the original post with post_id
	post *original_post = NULL;
	for (ll_node_t *node = lists_of_posts->head; node;
		 node = node->next) {
		post *current_post = (post *)node->data;
		if (current_post->id == post_id) {
			original_post = current_post;
			break;
		}
	}

	// Check friends' reposts
	ll_node_t *friend_node = friends_list->head;
	while (friend_node) {
		int friend_id = *(int *)friend_node->data;
		find_and_print_friends_reposts(original_post->events->root,
									   user_id, friend_id);
		friend_node = friend_node->next;
	}
}

linked_list_t *ll_copy(linked_list_t *list) {
	linked_list_t *copy = ll_create(sizeof(int));
	ll_node_t *node = list->head;
	while (node) {
		ll_add_nth_node(copy, ll_get_size(copy), node->data);
		node = node->next;
	}
	return copy;
}

ll_node_t *find_node1(linked_list_t *list, int data, unsigned int *pos) {
	ll_node_t *current = list->head;
	unsigned int index = 0;
	while (current) {
		if (*(int *)current->data == data) {
			if (pos)
				*pos = index;
			return current;
		}
		current = current->next;
		index++;
	}
	return NULL;
}

// function implemented from here
// https://en.wikipedia.org/wiki/Bron%E2%80%93Kerbosch_algorithm
// Surely the most hardest thing to do in this homework tbh
void bronkerbosch(int *R, int r_size, int *P, int p_size, int *X,
				  int x_size, list_graph_t *graph, int ***cliques,
				  int *clique_count, int *max_cliques_size)
{
	// if both P and X are empty, then R is a maximal clique
	if (p_size == 0 && x_size == 0) {
		// reallocing if needed
		if (*clique_count >= *max_cliques_size) {
			*max_cliques_size *= 2;
			*cliques = realloc(*cliques, (*max_cliques_size) * sizeof(int *));
		}
		(*cliques)[*clique_count] = malloc((r_size + 1) * sizeof(int));
		memcpy((*cliques)[*clique_count], R, r_size * sizeof(int));
		// the terminator
		(*cliques)[*clique_count][r_size] = -1;
		(*clique_count)++;
		return;
	}

	for (int i = 0; i < p_size; i++) {
		int v = P[i];

		// create a new set R' by adding v to R
		int *new_R = malloc((r_size + 1) * sizeof(int));
		memcpy(new_R, R, r_size * sizeof(int));
		new_R[r_size] = v;

		// create new sets P' and X' for the recursive call
		int *new_P = malloc(graph->nodes * sizeof(int));
		int new_p_size = 0;
		linked_list_t *neighbors = lg_get_neighbours(graph, v);
		ll_node_t *current = neighbors->head;
		while (current) {
			int neighbor = *(int *)current->data;
			//add neighbors of v that are in P to new_P
			for (int j = 0; j < p_size; j++) {
				if (P[j] == neighbor) {
					new_P[new_p_size++] = neighbor;
					break;
				}
			}
			current = current->next;
		}

		int *new_X = malloc(graph->nodes * sizeof(int));
		int new_x_size = 0;
		current = neighbors->head;
		while (current) {
			int neighbor = *(int *)current->data;
			// add neighbors of v that are in X to new_X
			for (int j = 0; j < x_size; j++) {
				if (X[j] == neighbor) {
					new_X[new_x_size++] = neighbor;
					break;
				}
			}
			current = current->next;
		}

		// recursive call with the new sets
		bronkerbosch(new_R, r_size + 1, new_P, new_p_size, new_X,
					 new_x_size, graph, cliques, clique_count,
					 max_cliques_size);

		free(new_R);
		free(new_P);
		free(new_X);

		// Remove v from P and add it to X
		P[i] = -1;
		X[x_size++] = v;
	}
}

int **find_maximal_cliques(list_graph_t *graph, int *clique_count) {
	int *R = malloc(graph->nodes * sizeof(int));
	int *P = malloc(graph->nodes * sizeof(int));
	int *X = malloc(graph->nodes * sizeof(int));
	for (int i = 0; i < graph->nodes; i++) {
		P[i] = i;
		X[i] = -1;
	}

	// finding the biggest cliques using bronkerbosch1
	int max_cliques_size = 10;
	int **cliques = malloc(max_cliques_size * sizeof(int *));
	*clique_count = 0;

	bronkerbosch(R, 0, P, graph->nodes, X, 0, graph, &cliques,
				 clique_count, &max_cliques_size);

	free(R);
	free(P);
	free(X);

	return cliques;
}

void free_cliques(int **cliques, int clique_count) {
	for (int i = 0; i < clique_count; i++)
		free(cliques[i]);
	free(cliques);
}

void common_group(list_graph_t *graph, char *user_name) {
	int user_id = get_user_id(user_name);

	int clique_count;
	int **cliques = find_maximal_cliques(graph, &clique_count);

	int *largest_clique = NULL;
	int largest_clique_size = 0;
	for (int i = 0; i < clique_count; i++) {
		int size = 0;
		int contains_user = 0;
		while (cliques[i][size] != -1) {
			if (cliques[i][size] == user_id)
				contains_user = 1;
			size++;
		}
		if (contains_user && size > largest_clique_size) {
			largest_clique_size = size;
			largest_clique = cliques[i];
		}
	}

	if (largest_clique_size == 0) {
		printf("No common group found.\n");
	} else {
		// Sort IDs in ascending order
		qsort(largest_clique, largest_clique_size, sizeof(int), compare_ints);

		printf("The closest friend group of %s is:\n", user_name);
		for (int i = 0; i < largest_clique_size; i++)
			printf("%s\n", get_user_name(largest_clique[i]));
	}

	free_cliques(cliques, clique_count);
}

void handle_input_feed(list_graph_t *graph, linked_list_t *lists_of_posts,
					   char *input)
{
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");

	if (!cmd) {
		free(commands);
		return;
	}

	if (!strcmp(cmd, "feed")) {
		char *user_name = strtok(NULL, " ");
		int feed_size = atoi(strtok(NULL, " "));
		feed_command(graph, user_name, feed_size, lists_of_posts);
	} else if (!strcmp(cmd, "view-profile")) {
		char *user_name = strtok(NULL, "\n");
		view_profile_command(user_name, lists_of_posts);
	} else if (!strcmp(cmd, "friends-repost")) {
		char *user_name = strtok(NULL, " ");
		int post_id = atoi(strtok(NULL, " "));
		friends_repost_command(graph, user_name, post_id, lists_of_posts);
	} else if (!strcmp(cmd, "common-group")) {
		char *user_name = strtok(NULL, "\n");
		common_group(graph, user_name);
	}

	free(commands);
}
