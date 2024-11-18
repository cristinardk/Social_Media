#ifndef FEED_H
#define FEED_H

//#include "feed.h"
#include "users.h"
#include "friends.h"
#include "linked_list.h"
#include "posts.h"

/**
 * Function that handles the calling of every command from task 3
 *
 * Please add any necessary parameters to the functions
*/
void handle_input_feed(list_graph_t *graph, linked_list_t *lists_of_posts,
					   char *input);

#endif // FEED_H
