/**
 * The entrypoint of the homework. Every initialization must be done here
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "users.h"
#include "friends.h"
#include "posts.h"
#include "feed.h"

/**
 * Initializez every task based on which task we are running
*/
void init_tasks(list_graph_t **graph, linked_list_t **lists_of_posts)
{
	#ifdef TASK_1
		*graph = lg_create(MAX_PEOPLE);
	#else
		*graph = NULL; //set to NULL if not used
	#endif

	#ifdef TASK_2
		*lists_of_posts = ll_create(sizeof(ll_node_t));
	#else
		*lists_of_posts = NULL; //set to NULL if not used
	#endif
}

/**
 * Entrypoint of the program, compiled with different defines for each task
*/
int main(void)
{
	init_users();
	list_graph_t *graph;
	linked_list_t *lists_of_posts;
	init_tasks(&graph, &lists_of_posts);

	#ifdef TASK_2
	int contor_id = 1;
	#endif
	char *input = (char *)malloc(MAX_COMMAND_LEN);
	while (1) {
		char *command = fgets(input, MAX_COMMAND_LEN, stdin);

		// If fgets returns null, we reached EOF
		if (!command)
			break;

		#ifdef TASK_1
		handle_input_friends(input, graph);
		#endif

		#ifdef TASK_2
		handle_input_posts(input, lists_of_posts, &contor_id);
		#endif

		#ifdef TASK_3
		handle_input_feed(graph, lists_of_posts, input);
		#endif
	}
	free_users();
	free(input);

	#ifdef TASK_1
		lg_free(graph);
	#endif

	#ifdef TASK_2
		free_posts_list(lists_of_posts);
	#endif

	return 0;
}
