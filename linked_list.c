#include "linked_list.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "posts.h"
linked_list_t *
ll_create(unsigned int data_size)
{
	linked_list_t *ll;

	ll = malloc(sizeof(*ll));

	ll->head = NULL;
	ll->data_size = data_size;
	ll->size = 0;

	return ll;
}

ll_node_t *get_nth_node(linked_list_t *list, unsigned int n)
{
	if (!list)
		return NULL;

	unsigned int len = list->size - 1;
	unsigned int i;
	ll_node_t *node = list->head;

	n = min(n, len);

	for (i = 0; i < n; ++i)
		node = node->next;

	return node;
}

void
ll_add_nth_node(linked_list_t *list, unsigned int n, const void *new_data)
{
	ll_node_t *prev, *curr;
	ll_node_t *new_node;

	if (!list)
		return;

	if (n > list->size)
		n = list->size;

	curr = list->head;
	prev = NULL;
	while (n > 0) {
		prev = curr;
		curr = curr->next;
		--n;
	}

	new_node = malloc(sizeof(*new_node));
	new_node->data = malloc(list->data_size);
	memcpy(new_node->data, new_data, list->data_size);
	new_node->next = curr;
	if (!prev)
		list->head = new_node;
	else
		prev->next = new_node;

	list->size++;
}

ll_node_t *
ll_remove_nth_node(linked_list_t *list, unsigned int n)
{
	ll_node_t *prev, *curr;

	if (!list || !list->head)
		return NULL;

	if (n > list->size - 1)
		n = list->size - 1;

	curr = list->head;
	prev = NULL;
	while (n > 0) {
		prev = curr;
		curr = curr->next;
		--n;
	}

	if (!prev)
		list->head = curr->next;
	else
		prev->next = curr->next;

	list->size--;

	return curr;
}

int
ll_get_size(linked_list_t *list)
{
	if (!list)
		return -1;

	return list->size;
}

void
ll_free(linked_list_t **pp_list)
{
	ll_node_t *curr_node;

	if (!pp_list || !*pp_list)
		return;

	while (ll_get_size(*pp_list) > 0) {
		curr_node = ll_remove_nth_node(*pp_list, 0);
		free(curr_node->data);
		curr_node->data = NULL;
		free(curr_node);
		curr_node = NULL;
	}

	free(*pp_list);
	*pp_list = NULL;
}
