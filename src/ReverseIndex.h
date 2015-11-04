#pragma once

#include <stdlib.h>
#include <string.h>
#include "LinkedList.h"

typedef struct DocumentOccurrence DocumentOccurrence;
struct DocumentOccurrence {
	char * doc_id;
	size_t count;
};

typedef struct ReverseIndex ReverseIndex;
typedef size_t (* ReverseIndexHash)(const char * key, size_t lenth);
struct ReverseIndex {
	LinkedList ** occurenceLists;
	size_t size;
	size_t capacity;
	ReverseIndexHash hash;
};

#define KEY_SIZE 20

DocumentOccurrence * newDocumentOccurrence(const char * doc_id, const size_t count) {
	DocumentOccurrence * occurrence = malloc(sizeof(DocumentOccurrence));
	occurrence->doc_id = calloc(sizeof(char), KEY_SIZE);
	memcpy(occurrence->doc_id, doc_id, KEY_SIZE);
	occurrence->count = count;
	return occurrence;
}
void deleteDocumentOccurrence(DocumentOccurrence * occurrence) {
	free(occurrence);
}
ReverseIndex * newReverseIndex(size_t entries, ReverseIndexHash hash) {
	ReverseIndex * indexTable = malloc(sizeof(ReverseIndex));
	indexTable->occurenceLists = calloc(entries, sizeof(LinkedList *));
	for (size_t i = 0; i < entries; ++i)
		indexTable->occurenceLists[i] = NULL;
	indexTable->size = 0;
	indexTable->capacity = entries;
	indexTable->hash = hash;
	return indexTable;
}
void deleteReverseIndex(ReverseIndex * indexTable) {
	for (size_t i = 0; i < indexTable->capacity; ++i)
		if (indexTable->occurenceLists[i])
			deleteLinkedList(indexTable->occurenceLists[i]);
	free(indexTable->occurenceLists);
	free(indexTable);
}
int frequentDocumentOccurrence(void * occurrence, void * occurrenceOnList) {
	return ((DocumentOccurrence *) occurrence)->count > ((DocumentOccurrence *) occurrenceOnList)->count;
}
const char * formatKey(const char * key) {
	char *validKey = malloc(20);
	int i;
	for (i = 0; key[i] != '\0'; ++i)
		validKey[i] = key[i];
	for (; i < 20; ++i)
		validKey[i] = 0;
	return validKey;
}
void insertDocumentOccurrence(ReverseIndex * indexTable, const char * key, DocumentOccurrence * occurrence) {
	key = formatKey(key);
	size_t index = indexTable->hash(key, 20) % indexTable->capacity;
	if (!indexTable->occurenceLists[index])
		indexTable->occurenceLists[index] = newLinkedList();
	insertElementOrdered(indexTable->occurenceLists[index], occurrence, frequentDocumentOccurrence, atLeft);
}
LinkedList * getDocumentOccurrence(ReverseIndex * indexTable, const char * key) {
	key = formatKey(key);
	size_t index = indexTable->hash(key, 20) % indexTable->capacity;
	return indexTable->occurenceLists[index];
}
