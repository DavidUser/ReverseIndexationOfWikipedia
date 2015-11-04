#include <stdio.h>
#include "ReverseIndex.h"

size_t hash(const char *key, size_t length) {
	int hashCode = 0;
	for (size_t i = 0; i < length; ++i)
		hashCode += key[i];
	return hashCode;
}

int main() {
	ReverseIndex * indexTable = newReverseIndex(2, hash);
	DocumentOccurrence * occurrence = newDocumentOccurrence("doc1", 5);
	insertDocumentOccurrence(indexTable, "word", occurrence);
	LinkedList * occurrenceList = getDocumentOccurrence(indexTable, "word");

	LinkedListIterator * it = newLinkedListIterator(occurrenceList);
	printf("word: ");
	while (hasNext(it)) {
		DocumentOccurrence * occurrence = (DocumentOccurrence *) getValue(it);
		printf("<%d, %s> ", occurrence->count, occurrence->doc_id);
	}
	printf("\n");

	return 0;
}
