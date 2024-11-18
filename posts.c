#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "users.h"
#include "posts.h"
#include "friends.h"

void free_tree(tree_t *tree);
void free_tree_node(tree_node_t *node);
tree_node_t *find_node_recursive(tree_node_t *node, int id);
tree_node_t *find_node(tree_t *tree, int id);

//function for creating a node .. in pid i will put the id of the posts/reposts
tree_node_t *create_tree_node(int pid, void *data) {
	tree_node_t *new_node = (tree_node_t *)calloc(1, sizeof(tree_node_t));
	new_node->pid = pid;
	new_node->data = data;
	return new_node;
}

//function for creating a tree
tree_t *create_tree(int data_size) {
	tree_t *new_tree = (tree_t *)calloc(1, sizeof(tree_t));
	new_tree->data_size = data_size;
	return new_tree;
}

// function for the list of posts
void ll_add_post_node(linked_list_t *list, unsigned int n, void *new_data) {
	ll_node_t *new_node = (ll_node_t *)malloc(sizeof(ll_node_t));
	new_node->data = new_data;

	if (n == 0 || !list->head) {
		new_node->next = list->head;
		list->head = new_node;
	} else {
		ll_node_t *prev = list->head;
		for (unsigned int i = 1; i < n && prev->next; ++i)
			prev = prev->next;
		new_node->next = prev->next;
		prev->next = new_node;
	}
	list->size++;
}

//function for creating a post
post *create_post(char *title, int uid, int *contor_id) {
	//allocating the post
	post *new_post = (post *)calloc(1, sizeof(post));

	//setting the initial parameters of the post
	new_post->id = (*contor_id)++;
	new_post->title = strdup(title);
	new_post->uid = uid;
	new_post->likes = 0;
	new_post->nr_liking_users = 0;

	//creating the tree for the eventual reposts
	new_post->events = create_tree(sizeof(tree_node_t));

	// in the root of the tree we will put the post itself in void* data
	new_post->events->root = create_tree_node(new_post->id, new_post);
	new_post->events->root->data = NULL;
	return new_post;
}

// function for handling the post command
void create_post_command(const char *user_name, char *title, int uid,
						 linked_list_t *lists_of_posts, int *contor_id) {
	//creating the post
	post *newpost = create_post(title, uid, contor_id);

	// adding the post to the list of posts
	ll_add_post_node(lists_of_posts, lists_of_posts->size, newpost);

	//printing the message
	printf("Created \"%s\" for %s\n", title, user_name);
}

// function for adding a child to a node
void add_child(tree_node_t *parent, tree_node_t *child) {
	parent->children = (tree_node_t **)realloc(parent->children,
											   (parent->n_children + 1) *
											   sizeof(tree_node_t *));
	parent->children[parent->n_children++] = child;
}

// function for creating a repost
post *create_repost(int uid, int *contor_id, char *original_title) {
	post *new_repost = (post *)calloc(1, sizeof(post));
	new_repost->id = (*contor_id)++;
	new_repost->title = original_title;
	new_repost->uid = uid;
	new_repost->likes = 0;

	// the only difference beetween this and the post is that events is nULL
	// because we dont store the reposts of a repost in the repost
	new_repost->events = NULL;
	return new_repost;
}

// FUNCTIONS FOR FINDING NODES
tree_node_t *find_node(tree_t *tree, int id) {
	// call the recursive function to find the node
	return find_node_recursive(tree->root, id);
}

tree_node_t *find_node_recursive(tree_node_t *node, int id) {
	if (node->pid == id)
		return node;
	for (int i = 0; i < node->n_children; i++) {
		tree_node_t *found = find_node_recursive(node->children[i], id);
		if (found)
			return found;
	}
	return NULL;
}

// function for handling the repost command
void repost_command(linked_list_t *lists_of_posts, const char *user_name,
					int post_id, int uid, int repost_id, int *contor_id) {
	post *original_post = NULL;
	// we search for the original post
	for (ll_node_t *node = lists_of_posts->head; node;
		 node = node->next) {
		post *current_post = (post *)node->data;
		if (current_post->id == post_id) {
			original_post = current_post;
			break;
		}
	}
	tree_node_t *original_post_node = find_node(original_post->events, post_id);

	// then we create the repost, and the node in the tree of the original post
	post *new_repost = create_repost(uid, contor_id, original_post->title);
	tree_node_t *new_repost_node = create_tree_node(new_repost->id,
													new_repost);

	// if repost_id is -1 it means that the repost is a direct repost of the
	// original post and we just add it
	if (repost_id == -1) {
		add_child(original_post_node, new_repost_node);
	} else {
		// if not we add it as a child of the respective repost
		tree_node_t *parent_repost_node = find_node(original_post->events,
													repost_id);
		add_child(parent_repost_node, new_repost_node);
	}
	printf("Created repost #%d for %s\n", new_repost->id, user_name);
}

// printing reposts
void print_reposts(tree_node_t *node, int level, int contor_id) {
	// so basically here is smth pretty awkward lol
	// When I had a repost of a repost it printed twice,
	// i think the reason was that the repost was added as a child of the
	// repost instead of the original post but I didn't change that
	// because it would have been more difficult to print the case
	// when I needed the repost of a repost
	// So i created a Romanian solution
	// A frequency vector which I use to check if I already printed the repost
	// and if I did I dont print it again
	// I know its not the best solution but it works LOL
	int *freq_vector = malloc(contor_id * sizeof(int));
	for (int i = 0; i < contor_id; i++)
		freq_vector[i] = 0;

	post *post_data = (post *)node->data;
	for (int i = 0; i < level; ++i) {
		if (freq_vector[post_data->id] == 0) {
			freq_vector[post_data->id] = 1;
			printf("Repost #%d by %s\n", post_data->id,
				   get_user_name(post_data->uid));
		}
	}

	for (int i = 0; i < node->n_children; ++i)
		print_reposts(node->children[i], level + 1, contor_id);

	free(freq_vector);
}

// function for handling the get reposts command
void get_reposts_command(linked_list_t *lists_of_posts, int post_id,
						 int repost_parent_id, int *contor_id) {
	post *original_post = NULL;
	// searching for the original post
	for (ll_node_t *node = lists_of_posts->head; node;
		 node = node->next) {
		post *current_post = (post *)node->data;
		if (current_post->id == post_id) {
			original_post = current_post;
			break;
		}
	}

	// if repost_parent_id is -1 means we have to print the reposts of the
	// original post if not repost of repost
	if (repost_parent_id == -1) {
		printf("\"%s\" - Post by %s\n", original_post->title,
			   get_user_name(original_post->uid));
		print_reposts(original_post->events->root, 0, *contor_id);
	} else {
		tree_node_t *repost_parent = NULL;
		repost_parent = find_node(original_post->events, repost_parent_id);
		post *post_data = (post *)repost_parent->data;
		printf("Repost #%d by %s\n", post_data->id,
			   get_user_name(post_data->uid));
	}
}

void free_tree_node(tree_node_t *node) {
	for (int i = 0; i < node->n_children; i++)
		free_tree_node(node->children[i]);
	if (node->data)
		free(node->data);
	free(node->children);
	free(node);
}

void free_tree(tree_t *tree) {
	free_tree_node(tree->root);
	free(tree);
}

void free_post(post *p) {
	if (p->title)
		free(p->title);
	if (p->events)
		free_tree(p->events);
	free(p);
}

void free_posts_list(linked_list_t *lists_of_posts) {
	ll_node_t *current = lists_of_posts->head;
	ll_node_t *next;
	while (current) {
		next = current->next;
		post *p = (post *)current->data;
		free_post(p);
		free(current);
		current = next;
	}
	free(lists_of_posts);
}

void like_post(post *p, int user_id) {
	// Check if the user has already liked the post
	for (int i = 0; i < p->nr_liking_users; i++) {
		if (p->liking_users[i] == user_id) {
			// User has already liked the post, so unlike it
			for (int j = i; j < p->nr_liking_users - 1; j++)
				p->liking_users[j] = p->liking_users[j + 1];
			p->nr_liking_users--;
			p->likes--;
			if (p->events) {
				printf("User %s unliked post \"%s\"\n",
					   get_user_name(user_id), p->title);
			} else {
				printf("User %s unliked repost \"%s\"\n",
					   get_user_name(user_id), p->title);
			}
			return;
		}
	}
	// User has not liked the post, so add the like
	p->liking_users[p->nr_liking_users++] = user_id;
	p->likes++;
	if (p->events) {
		printf("User %s liked post \"%s\"\n",
			   get_user_name(user_id), p->title);
	} else {
		printf("User %s liked repost \"%s\"\n",
			   get_user_name(user_id), p->title);
	}
}

void like_command(linked_list_t *lists_of_posts, int post_id,
				  int repost_id, int uid)
{
	post *target_post = NULL;

	for (ll_node_t *node = lists_of_posts->head; node;
		 node = node->next) {
		post *current_post = (post *)node->data;
		if (current_post->id == post_id) {
			target_post = current_post;
			break;
		}
	}

	if (repost_id == -1) {
		like_post(target_post, uid);
	} else {
		tree_node_t *repost_node = find_node(target_post->events, repost_id);
		post *repost_data = (post *)repost_node->data;
		like_post(repost_data, uid);
	}
}

void get_likes_command(linked_list_t *lists_of_posts, int post_id,
					   int repost_id)
{
	post *original_post = NULL;

	for (ll_node_t *node = lists_of_posts->head; node;
		 node = node->next) {
		post *current_post = (post *)node->data;
		if (current_post->id == post_id) {
			original_post = current_post;
			break;
		}
	}

	if (repost_id == -1) {
		printf("Post \"%s\" has %d likes\n", original_post->title,
			   original_post->likes);
	} else {
		tree_node_t *repost_node = find_node(original_post->events, repost_id);
		post *repost_data = (post *)repost_node->data;
		printf("Repost #%d has %d likes\n", repost_data->id,
			   repost_data->likes);
	}
}

post *find_most_liked_repost(tree_node_t *root)
{
	// we will use a queue to traverse the tree
	// in searching of the most liked post
	tree_node_t **queue = malloc(MAX_LIKING_USERS * sizeof(tree_node_t *));
	int front = 0, rear = 0;
	queue[rear++] = root;

	post *most_liked_post = NULL;

	// Here we will traverse the tree
	while (front < rear) {
		tree_node_t *current = queue[front++];
		post *current_post = (post *)current->data;

		// if the current node is a post we check if it is the most liked
		// post
		if (current_post) {
			// performing the switching
			if (!most_liked_post || current_post->likes >
				most_liked_post->likes ||
				(current_post->likes == most_liked_post->likes &&
				current_post->id < most_liked_post->id))
				most_liked_post = current_post;
		}

		for (int i = 0; i < current->n_children; i++)
			queue[rear++] = current->children[i];
	}

	free(queue);
	// in most_liked_post we will have the most liked post
	return most_liked_post;
}

// finding the common ancestor of two reposts
tree_node_t *find_common_ancestor(tree_node_t *root, tree_node_t *node1,
								  tree_node_t *node2)
{
	// basically if the root is one of the nodes we return the root lol
	if (root == node1 || root == node2)
		return root;

	int count = 0;
	tree_node_t *temp = NULL;

	// searching for the common ancestor
	for (int i = 0; i < root->n_children; i++) {
		tree_node_t *res = find_common_ancestor(root->children[i],
												node1, node2);
		if (res) {
			count++;
			temp = res;
		}
		// if we found 2 nodes in the children of the root
		// it means that the root is the common ancestor
		if (count == 2)
			return root;
	}
	return temp;
}

void common_repost_command(linked_list_t *lists_of_posts, int post_id,
						   int repost_id1, int repost_id2)
{
	post *original_post = NULL;
	for (ll_node_t *node = lists_of_posts->head; node;
		 node = node->next) {
		post *current_post = (post *)node->data;
		if (current_post->id == post_id) {
			original_post = current_post;
			break;
		}
	}
	// finding the 2 reposts then searching for the common ancestor
	tree_node_t *repost1_node = find_node(original_post->events, repost_id1);
	tree_node_t *repost2_node = find_node(original_post->events, repost_id2);

	tree_node_t *common_ancestor = find_common_ancestor(original_post->events
														->root, repost1_node,
														repost2_node);

	if (common_ancestor && common_ancestor->data) {
		post *common_post = (post *)common_ancestor->data;
		printf("The first common repost of %d and %d is %d\n",
			   repost_id1, repost_id2, common_post->id);
	} else {
		printf("The first common repost of %d and %d is %d\n",
			   repost_id1, repost_id2, original_post->id);
	}
}

void ratio_command(linked_list_t *lists_of_posts, int post_id) {
	post *original_post = NULL;

	for (ll_node_t *node = lists_of_posts->head; node;
		 node = node->next) {
		post *current_post = (post *)node->data;
		if (current_post->id == post_id) {
			original_post = current_post;
			break;
		}
	}

	post *most_liked_repost = NULL;
	most_liked_repost = find_most_liked_repost(original_post->events->root);

	if (most_liked_repost && most_liked_repost->likes > original_post->likes) {
		printf("Post %d got ratio'd by repost %d\n",
			   original_post->id, most_liked_repost->id);
	} else {
		printf("The original post is the highest rated\n");
	}
}

//function aiding in deleting
void remove_child(tree_node_t *parent, tree_node_t *child)
{
	// it basically removes a child not much to say
	// BUT i think from here comes the problem on the last test task3
	// I think pointing to parent->children[j+1] doesn't work as I think
	// because I store the children in repost->children instead of
	// original_post->children
	for (int i = 0; i < parent->n_children; i++) {
		if (parent->children[i] == child) {
			for (int j = i; j < parent->n_children - 1; j++)
				parent->children[j] = parent->children[j + 1];
			parent->n_children--;
			parent->children = realloc(parent->children, parent->n_children *
									   sizeof(tree_node_t *));
			return;
		}
	}
}

void delete_post_command(linked_list_t *lists_of_posts,
						 int post_id, int repost_id)
{
	post *original_post = NULL;
	ll_node_t *prev = NULL;
	for (ll_node_t *node = lists_of_posts->head; node;
		 node = node->next) {
		post *current_post = (post *)node->data;
		if (current_post->id == post_id) {
			original_post = current_post;
			break;
		}
		prev = node;
	}

	if (repost_id == -1) {
		if (prev)
			prev->next = prev->next->next;
		else
			lists_of_posts->head = lists_of_posts->head->next;
		lists_of_posts->size--;
		printf("Deleted \"%s\"\n", original_post->title);
		free_post(original_post);
	} else {
		tree_node_t *repost_node = find_node(original_post->events, repost_id);
		tree_node_t *parent_node =
		find_node_recursive(original_post->events->root, post_id);
		if (parent_node)
			remove_child(parent_node, repost_node);
		printf("Deleted repost #%d of post \"%s\"\n", repost_node->pid,
			   ((post *)repost_node->data)->title);
		free_tree_node(repost_node);
	}
}

void handle_input_posts(char *input, linked_list_t *lists_of_posts,
						int *contor_id)
{
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");

	if (!cmd)
		return;

	if (!strcmp(cmd, "create")) {
		char *user_name = strtok(NULL, " ");
		char *title = strtok(NULL, "\"");
		int uid = get_user_id(user_name);
		create_post_command(user_name, title, uid, lists_of_posts, contor_id);
	} else if (!strcmp(cmd, "repost")) {
		char *user_name = strtok(NULL, " ");
		int post_id = atoi(strtok(NULL, " "));
		char *repost_id_str = strtok(NULL, " ");
		int repost_id = (repost_id_str) ? atoi(repost_id_str) : -1;
		int uid = get_user_id(user_name);
		repost_command(lists_of_posts, user_name, post_id, uid,
					   repost_id, contor_id);
	} else if (!strcmp(cmd, "common-repost")) {
		char *post_id_str = strtok(NULL, " ");
		int post_id = atoi(post_id_str);
		char *repost_id1_str = strtok(NULL, " ");
		int repost_id1 = atoi(repost_id1_str);
		char *repost_id2_str = strtok(NULL, " ");
		int repost_id2 = atoi(repost_id2_str);
		common_repost_command(lists_of_posts, post_id, repost_id1, repost_id2);
	} else if (!strcmp(cmd, "like")) {
		char *user_name = strtok(NULL, " ");
		int uid = get_user_id(user_name);
		char *post_id_str = strtok(NULL, " ");
		int post_id = atoi(post_id_str);
		int repost_id = -1;
		char *repost_id_str = strtok(NULL, " ");
		repost_id = (repost_id_str) ? atoi(repost_id_str) : -1;
		like_command(lists_of_posts, post_id, repost_id, uid);
	} else if (!strcmp(cmd, "ratio")) {
		char *post_id_str = strtok(NULL, " ");
		int post_id = atoi(post_id_str);
		ratio_command(lists_of_posts, post_id);
	} else if (!strcmp(cmd, "delete")) {
		char *post_id_str = strtok(NULL, " ");
		int post_id = atoi(post_id_str);
		char *repost_id_str = strtok(NULL, " ");
		int repost_id = (repost_id_str) ? atoi(repost_id_str) : -1;
		delete_post_command(lists_of_posts, post_id, repost_id);
	} else if (!strcmp(cmd, "get-likes")) {
		int post_id = atoi(strtok(NULL, " "));
		char *repost_id_str = strtok(NULL, " ");
		int repost_id = (repost_id_str) ? atoi(repost_id_str) : -1;
		get_likes_command(lists_of_posts, post_id, repost_id);
	} else if (!strcmp(cmd, "get-reposts")) {
		char *post_id_str = strtok(NULL, " ");
		int post_id = atoi(post_id_str);
		int repost_parent_id = -1;
		char *repost_parent_id_str = strtok(NULL, " ");
		repost_parent_id = (repost_parent_id_str) ?
							atoi(repost_parent_id_str) : -1;
		get_reposts_command(lists_of_posts, post_id, repost_parent_id,
							contor_id);
	}
	if (commands)
		free(commands);
}
