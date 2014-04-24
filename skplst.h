#ifndef SKPLST_H
#define SKPLST_H

struct skplst_iter {
	struct skplst_elem *elem;
};

struct skplst_elem {
	int levels;
	void *data;
	struct skplst_elem **next;
};

struct skplst {
	float prob;
	int max_levels;
	int (*cmp_elem)(void*, void*);
	struct skplst_elem **next;
	struct skplst_elem ***search_path;
};

int skplst_insert(struct skplst *list, void *data);
int skplst_delete(struct skplst *list, void *data);
void *skplst_find(struct skplst *list, void *data);
void skplst_free(struct skplst *list);
void skplst_foreach(struct skplst *list, void (*callback)(void*));
void skplst_init(struct skplst *list, int max_levels, float prob, int (*cmp_elem)(void*, void*));

void skplst_iter_init(struct skplst_iter *, struct skplst *, void *data);
void skplst_iter_next(struct skplst_iter *it);
void *skplst_iter_data(struct skplst_iter *it);

#endif
