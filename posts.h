#ifndef POSTS_H
#define POSTS_H

#include "linked_list.h"
#define MAX_STRING_SIZE	1000

#define MAX_LIKING_USERS 100

typedef struct tree_node_t {
	struct tree_node_t **children;
	int n_children;
	int pid;
	void *data;
} tree_node_t;

typedef struct tree_t {
	tree_node_t *root;
	int data_size;
} tree_t;

typedef struct post {
	int id;
	char *title;
	int uid;
	tree_t *events;
	int nr_liking_users;
	int liking_users[MAX_LIKING_USERS];
	int likes;
} post;

/**
 * Function that handles the calling of every command from task 2
 *
 * Please add any necessary parameters to the functions
*/
void handle_input_posts(char *input, linked_list_t *lists_of_posts,
						int *contor_id);
void free_posts_list(linked_list_t *lists_of_posts);
#endif // POSTS_H
