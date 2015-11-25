#pragma once

#include <stdlib.h>
#include <string.h>

typedef int (* CompareFunction)(void *, void *);
typedef struct Node Node;
typedef struct LinkedList LinkedList;

struct Node {
	void * value;
	Node * next;
	Node * previous;
};
Node * newNode(void * value, size_t size) {
	Node * node = malloc(sizeof(Node));
	node->value = malloc(size);
	memcpy(node->value, value, size);
	node->next = 0;
	node->previous = 0;
}
void deleteNode(Node * node) {
	free(node->value);
	free(node);
}

struct LinkedList {
	Node * start;
	Node * end;
	size_t size;
};
LinkedList * newLinkedList() {
	LinkedList * list = malloc(sizeof(LinkedList));
	list->start = 0;
	list->end = 0;
	list->size = 0;
	return list;
}
void deleteLinkedList(LinkedList * list) {
	Node * i = list->start;
	while (i) {
		Node * next = i->next;
		deleteNode(i);
		i = next;
	}
	free(list);
}
		

void insertNextNode(LinkedList * list, Node * previous, Node * node) {
	if (previous) {
		node->previous = previous;
		Node * next = previous->next;
		previous->next = node;
		if (next) {
			node->next = next;
			next->previous = node;
		}
		if (list->end == previous)
			list->end = node;
	} else {
		list->start = node;
		list->end = node;
	}
	++(list->size);
}
void insertPreviousNode(LinkedList * list, Node * next, Node * node) {
	if (!next)
		insertNextNode(list, next, node);
	else if (next->previous)
		insertNextNode(list, next->previous, node);
	else {
		list->start = node;
		node->next = next;
		next->previous = node;
		++(list->size);
	}
}
Node *searchNode(LinkedList * list, void * value,  CompareFunction compare) {
	for (Node * i = list->start; i; i = i->next)
		if (compare(value, i->value))
		       return i;
	return 0;
}	
enum Side { atLeft = 0, atRight = 1};
void insertStructElementOrdered(LinkedList * list, void * value, size_t valueSize, CompareFunction compare, int side) {
	Node * found = searchNode(list, value, compare);
	if (!found && list->size > 0)
		found = list->end;
	Node * node = newNode(value, valueSize);
	if (side == 0)
		insertPreviousNode(list, found, node);
	else
		insertNextNode(list, found, node);
}
#define insertElementOrdered(LIST, VALUE, COMPARE, SIDE) insertStructElementOrdered(LIST, &VALUE, sizeof(VALUE), COMPARE, SIDE)
void pushBackStructElement(LinkedList * list, void * value, size_t valueSize) {
	Node * node = newNode(value, valueSize);
	insertNextNode(list, list->end, node);
}
#define pushBackElement(LIST, VALUE) pushBackStructElement(LIST, &VALUE, sizeof(VALUE))
void removeNode(LinkedList *list, Node * node) {
	if (!node)
		return;
	if (list->start == node)
		list->start = node->next;
	if (list->end == node)
		list->end = node->previous;
	if (node->previous)
		node->previous->next = node->next;
	if (node->next)
		node->next->previous = node->previous;
	deleteNode(node);
}
void removeElement(LinkedList *list, void * value, CompareFunction compare) {
	Node * found = searchNode(list, value, compare);
	removeNode(list, found);
}
void * searchElement(LinkedList *list, void * value, CompareFunction compare) {
	Node * found = searchNode(list, value, compare);
	return found? found->value : 0;
}

typedef struct LinkedListIterator LinkedListIterator;
struct LinkedListIterator {
	LinkedList * list;
	Node * current;
};
LinkedListIterator * newLinkedListIterator(LinkedList * list) {
	LinkedListIterator * iterator = malloc(sizeof(LinkedListIterator));
	iterator->list = list;
	iterator->current = list->start;
}
int hasNext(LinkedListIterator * iterator) {
	return iterator->current != 0;
}
void * getValue(LinkedListIterator * iterator) {
	void * value = iterator->current->value;
	iterator->current = iterator->current->next;
	return value;
}
