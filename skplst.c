#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "skplst.h"

static int skplst_coin_flip_levels(struct skplst *list);
static struct skplst_elem **skl_next_alloc(int levels);
static struct skplst_elem *skplst_elem_create(struct skplst *list, void *data);
static void skplst_elem_free(struct skplst_elem* to_free);
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

struct skplst_elem **skl_next_alloc(int levels) {
	struct skplst_elem **new_next = (struct skplst_elem**)malloc(sizeof(struct skplst_elem*)*levels);
	memset(new_next, 0, levels*sizeof(struct skplst_elem**));
	return new_next;
}

struct skplst_elem *skplst_elem_create(struct skplst *list, void *data) {
	struct skplst_elem *new = (struct skplst_elem*)malloc(sizeof(struct skplst_elem));
	new->levels = skplst_coin_flip_levels(list);
	new->data = data;
	new->next = skl_next_alloc(new->levels);
	return new;
}

void skplst_elem_free(struct skplst_elem* to_free) {
	free(to_free->next);
	free(to_free);
}

int skplst_insert(struct skplst *list, void *data) {
	struct skplst_elem **next = list->next;
	int i;
	for (i = list->max_levels - 1; i >= 0;) {
		int cmp;
		if (next[i] && (cmp = list->cmp_elem(data, next[i]->data)) >= 0) {
			if (cmp == 0)
				return 0;
			next = next[i]->next;
		} else {
			list->search_path[i] = &next[i];
			i--;
		}
	}

	struct skplst_elem *new_elem = skplst_elem_create(list, data);
	for (i = 0; i < new_elem->levels; i++) {
		new_elem->next[i] = *list->search_path[i];
		*list->search_path[i] = new_elem;
	}
	return 1;
}

int skplst_delete(struct skplst *list, void *data) {
	struct skplst_elem **next = list->next;
	int i;
	for (i = list->max_levels - 1; i >= 0;) {
		if (next[i] && list->cmp_elem(data, next[i]->data) > 0) {
			next = next[i]->next;
		} else {
			list->search_path[i] = &next[i];
			i--;
		}
	}
	struct skplst_elem *to_delete = next[0];
	if (!to_delete || list->cmp_elem(data, to_delete->data) != 0)
		return 0;

	for (i = 0; i < to_delete->levels; i++)
		*list->search_path[i] = to_delete->next[i];

	skplst_elem_free(to_delete);
	return 1;
}

struct skplst_elem *skplst_find_elem(struct skplst *list, void *data) {
	struct skplst_elem **next = list->next;
	int levels = list->max_levels;
	int i;
	for (i = levels - 1; i >= 0;) {
		int cmp;
		if (next[i] && (cmp = list->cmp_elem(data, next[i]->data)) >= 0) {
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
		skplst_elem_free(elem);
		elem = tmp;
	}
	free(list->search_path);
	free(list->next);
}

void skplst_foreach(struct skplst *list, void (*callback)(void*)) {
	struct skplst_elem *elem = list->next[0];
	while (elem) {
		callback(elem->data);
		elem = elem->next[0];
	}
}

void skplst_init(struct skplst *list, int max_levels,
					float prob, int (*cmp_elem)(void*, void*)) {
	list->prob = prob;
	list->max_levels = max_levels;
	list->cmp_elem = cmp_elem;

	list->next = skl_next_alloc(max_levels);
	list->search_path = (struct skplst_elem***)malloc(sizeof(struct skplst_elem**)*max_levels);
	srand48(time(NULL));
}
