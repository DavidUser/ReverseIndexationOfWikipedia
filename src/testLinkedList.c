#include <stdio.h>
#include "LinkedList.h"

int great(void * x, void * y) {
	return *((int *) x) > *((int *) y);
}

int equal(void *x, void *y) {
	return *((int *) x) == *((int *) y);
}

int main() {
	LinkedList * list = newLinkedList();

	for (int x = 0; x < 10; ++x)
		insertElementOrdered(list, &x, great, atLeft);
	int key = 5;
	removeElement(list, &key, equal);
	int keyChange = 9;
	int * element = searchElement(list, &keyChange, equal);
	*element *= 10;

	LinkedListIterator * it = newLinkedListIterator(list);
	while (hasNext(it))
		printf("%d\n", *((int *) getValue(it)));

	deleteLinkedList(list);

	return 0;
}
