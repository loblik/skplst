#include <stdio.h>
#include "skplst.h"

struct pair { char *key; int value; };

int str_compare(const void *k1, const void *k2, void *data) {
	struct pair *p1 = (struct pair*)k1;
	struct pair *p2 = (struct pair*)k2;
	return strcmp((char*)p1->key, (char*)p2->key);
}

int main(int argc, char **argv) {

	struct skplst list;
	skplst_init(&list, 3, 0.25, &str_compare, NULL, NULL, NULL);

	struct pair test[] = {
		{ "key4",  9 },
		{ "key1",  4 },
		{ "key3",  7 },
		{ "key2", 11 },
	};

	int i;
	for (i = 0; i < 4; i++)
		skplst_insert(&list, &test[i]);

	skplst_delete(&list, &test[0]);

	struct skplst_iter it;
	struct pair *data;
	skplst_iter_init(&it, &list, &test[3]);
	for (; (data = (struct pair*)skplst_iter_data(&it)); skplst_iter_next(&it))
		printf("%s %d\n", data->key, data->value);

	skplst_free(&list);
	return 0;
}
