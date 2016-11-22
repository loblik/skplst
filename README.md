# skplst - skip list C library

This is a simple C library implementing skip list datastructure. It was written as semestral work for advanced algorithms course. You can read more on skip lists on [wikipedia](https://en.wikipedia.org/wiki/Skip_list). See below how to use this library.

# How to use it

## Basic example with integers

### Comparison callback function
At first we need some function to compare our data. This function is called by the library anytime it needs to compare two items. The convention is the same as used by strcmp() in stdlib. 
```
#!c
int int_compare(void *a, void *b) {
    int va = *(int*)a;
    int vb = *(int*)b;
    if (va == vb) 
        return 0;
    else
        return va < vb ? -1 : 1;
}
```
If *a* is less than *b* then the callback function should return a negative number, if *b* is greater *a* then it should return positive and zero is returned if the items are equal.

### Initialization
Once we have a callback function we can create a new skiplist. 
```
#!c
struct skplst list;
skplst_init(&list, 16, 0.25, &int_compare);
```
The first argument is a pointer to allocated memory for skiplist structure, the second is the maximal allowed levels in the skiplist, the third is probability of using one more level while inserting new item and the last one is pointer to our comparison function. 

### Inserting values
Inserting values is quite easy.
```
#!c
int values[] = { 3, 1, 5, 6, 9, 2 };
```
Simply pass pointer to you your data to the insert function.
```
#!c
int i;
for (i = 0; i < 6; i++)
    skplst_insert(&list, &values[i]);
```
###Iterate the list
You can use iterator to walk through all items in the list.
```
#!c
int *val;
struct skplst_iter it;
```
If you pass NULL to skplst_iter_init then iterator will point to the first item.
```
#!c
skplst_iter_init(&it, &list, NULL);
```

Or you can set it at any item in the list by passing pointer to it.
```
#!c
skplst_iter_init(&it, &list, &values[0]);
```
And then go through the list with a simple loop.
```
#!c
for (; (val = (int*)skplst_iter_data(&it)); skplst_iter_next(&it))
    printf("%d\n", *val);
```
At the end you should realease the list. Note this frees only the list, it doesn't free your data. If you have heap allocated data you have to itereate over items and free it yourself and then call this function to free the list itself.
```
#!c
skplst_free(&list);
```
## Example with key value pairs

If you need key value mapping you can include key in the data structure and let the callback to compare the keys as it's shown in the following example.

```
#!c
struct pair { char *key; int value; };

int str_compare(void *k1, void *k2) {
    struct pair *p1 = (struct pair*)k1;
    struct pair *p2 = (struct pair*)k2;
    return strcmp((char*)p1->key, (char*)p2->key);
}

int main(void) {

    struct skplst list;
    skplst_init(&list, 3, 0.25, &str_compare);

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
```
