#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "skplst.h"

static int skplst_coin_flip_levels(struct skplst *list);
static struct skplst_elem **skl_next_alloc(struct skplst *list, int levels);
static struct skplst_elem *skplst_elem_create(struct skplst *list, int *levs, void *data);
static void skplst_elem_free(struct skplst *list, struct skplst_elem* to_free);
static struct skplst_elem *skplst_find_elem(struct skplst *list, void *data);

void skplst_iter_init(struct skplst_iter *it, struct skplst *list, void *data) {
	if (data)
		it->elem = skplst_find_elem(list, data);
	else
		it->elem = list->next[0];
}

void *skplst_iter_data(struct skplst_iter *it) {
	if (it->elem)
		return it->elem->data;
	else
		return NULL;
}

void skplst_iter_next(struct skplst_iter *it) {
	if (it->elem)
		it->elem = it->elem->next[0];
}

int skplst_coin_flip_levels(struct skplst *list) {
	int levels = 1;
	while (drand48() < list->prob && levels < list->max_levels)
		levels++;
	return levels;
}

struct skplst_elem **skl_next_alloc(struct skplst *list, int levels) {
	struct skplst_elem **new_next =
		(struct skplst_elem**)list->malloc_fnc(sizeof(struct skplst_elem*)*levels);
	memset(new_next, 0, levels*sizeof(struct skplst_elem**));
	return new_next;
}

struct skplst_elem *skplst_elem_create(struct skplst *list, int *levs, void *data) {
	int levels = skplst_coin_flip_levels(list);
	struct skplst_elem *new =
		(struct skplst_elem*)list->malloc_fnc(sizeof(struct skplst_elem) +
				sizeof(struct skplst_elem*)*(levels - 1));
	*levs = levels;
	new->data = data;
	memset(new->next, 0, levels*sizeof(struct skplst_elem*));
	return new;
}

void skplst_elem_free(struct skplst *list, struct skplst_elem* to_free) {
	list->free_fnc(to_free);
}

void *skplst_insert(struct skplst *list, void *data) {
	struct skplst_elem **next = list->next;
	int i;
	for (i = list->max_levels - 1; i >= 0;) {
		int cmp;
		if (next[i] && (cmp = list->cmp_elem(data, next[i]->data, list->user_data)) >= 0) {
			if (cmp == 0)
				return NULL;
			next = next[i]->next;
		} else {
			list->search_path[i] = &next[i];
			i--;
		}
	}
	int levels;
	struct skplst_elem *new_elem = skplst_elem_create(list, &levels, data);
	for (i = 0; i < levels; i++) {
		new_elem->next[i] = *list->search_path[i];
		*list->search_path[i] = new_elem;
	}
	return data;
}

void *skplst_delete(struct skplst *list, void *data) {
	struct skplst_elem **next = list->next;
	int i;
	for (i = list->max_levels - 1; i >= 0;) {
		if (next[i] && list->cmp_elem(data, next[i]->data, list->user_data) > 0) {
			next = next[i]->next;
		} else {
			list->search_path[i] = &next[i];
			i--;
		}
	}
	struct skplst_elem *to_delete = next[0];
	if (!to_delete || list->cmp_elem(data, to_delete->data, list->user_data) != 0)
		return NULL;

	for (i = 0; i < list->max_levels && *list->search_path[i] == to_delete; i++)
		*list->search_path[i] = to_delete->next[i];

	void *deleted_data = to_delete->data;
	skplst_elem_free(list, to_delete);
	return deleted_data;
}

struct skplst_elem *skplst_find_elem(struct skplst *list, void *data) {
	struct skplst_elem **next = list->next;
	int levels = list->max_levels;
	int i;
	for (i = levels - 1; i >= 0;) {
		int cmp;
		if (next[i] && (cmp = list->cmp_elem(data, next[i]->data, list->user_data)) >= 0) {
			if (cmp == 0)
				return next[i];
			next = next[i]->next;
		} else {
			i--;
		}
	}
	return NULL;
}


void *skplst_find(struct skplst *list, void *data) {
	struct skplst_elem *elem = skplst_find_elem(list, data);
	if (elem)
		return elem->data;
	return NULL;
}

void skplst_free(struct skplst *list) {
	struct skplst_elem *elem = list->next[0];
	struct skplst_elem *tmp;
	while (elem) {
		tmp = elem->next[0];
		skplst_elem_free(list, elem);
		elem = tmp;
	}
	list->free_fnc(list->search_path);
	list->free_fnc(list->next);
}

void skplst_foreach(struct skplst *list, void (*callback)(void*)) {
	struct skplst_elem *elem = list->next[0];
	while (elem) {
		callback(elem->data);
		elem = elem->next[0];
	}
}

void skplst_init(struct skplst *list, int max_levels,
					float prob, int (*cmp_elem)(const void*, const void*, void *), void *user_data,
					void *(*malloc_fnc)(size_t), void (*free_fnc)(void*)) {
	list->prob = prob;
	list->max_levels = max_levels;
	list->cmp_elem = cmp_elem;
	list->user_data = user_data;

	list->malloc_fnc = malloc_fnc;
	list->free_fnc = free_fnc;

	if (list->malloc_fnc == NULL)
		list->malloc_fnc = &malloc;

	if (list->free_fnc == NULL)
		list->free_fnc = &free;

	list->next = skl_next_alloc(list, max_levels);
	list->search_path = (struct skplst_elem***)list->malloc_fnc(sizeof(struct skplst_elem**)*max_levels);
	srand48(time(NULL));
}
