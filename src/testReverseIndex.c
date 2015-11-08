#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include "ReverseIndex.h"

size_t hash(const char *key, size_t length) {
	int hashCode = 0;
	for (size_t i = 0; i < length; ++i)
		hashCode += key[i];
	return hashCode;
}

void showReverseIndex(ReverseIndex * indexTable, const char *key) {
	printf("%s: ", key);
	LinkedList * occurrenceList = getDocumentOccurrence(indexTable, key);

	if (occurrenceList) {
		LinkedListIterator * it = newLinkedListIterator(occurrenceList);
		while (hasNext(it)) {
			DocumentOccurrence * occurrence = (DocumentOccurrence *) getValue(it);
			printf("<%u, %s> ", occurrence->count, occurrence->doc_id);
		}
	}
	printf("\n");
}


void testBasicStructure() {
	ReverseIndex * indexTable = newReverseIndex(60, hash);
	DocumentOccurrence * occurrence = newDocumentOccurrence("doc2", 5);
	DocumentOccurrence * occurrence2 = newDocumentOccurrence("doc2", 6);
	DocumentOccurrence * occurrence3 = newDocumentOccurrence("doc1", 6);
	DocumentOccurrence * occurrenceB0 = newDocumentOccurrence("doc1", 9);
	// TODO colision treatment!
	insertDocumentOccurrence(indexTable, "cat", occurrence);
	insertDocumentOccurrence(indexTable, "cat", occurrence2);
	insertDocumentOccurrence(indexTable, "cat", occurrence3);
	insertDocumentOccurrence(indexTable, "dog", occurrenceB0);

	showReverseIndex(indexTable, "cat");
	showReverseIndex(indexTable, "dog");
	showReverseIndex(indexTable, "bird");
}

void testFillReverseIndex() {
	ReverseIndex * indexTable = newReverseIndex(60, hash);
	fillReverseIndex(indexTable, "cat dog cat", "doc1");
	fillReverseIndex(indexTable, "dog dog dog", "doc1");
	fillReverseIndex(indexTable, "dog dog dog", "doc2");
	fillReverseIndex(indexTable, "dog dog cat", "doc2");

	showReverseIndex(indexTable, "cat");
	showReverseIndex(indexTable, "dog");
	showReverseIndex(indexTable, "bird");
}

char wcharToChar(wint_t wc) {
	wchar_t *wide = "àâêôûãõáéíóúçüÀÂÊÔÛÃÕÁÉÍÓÚÇÜ";
	char *asc =	 "aaeouaoaeioucuAAEOUAOAEIOUCU";

	for (size_t i = 0; i < sizeof(wide); ++i)
		if (wc == wide[i])
			return asc[i];
}

void testFillReverseIndexInputed() {
	ReverseIndex * indexTable = newReverseIndex(60, hash);

	printf("do you want insert a file? ");
	char op = getchar();
	while (op == 'y') {
		char name[20];
		printf("file name: ");
		scanf("%s", name);
		FILE * file = fopen(name, "r");
		if (file) {
			while (!feof(file)) {
				char buffer[255];
				size_t bufferSize = 0;
				size_t lastSeparator = 0;
				for (char ch = wcharToChar(getwc(file)) ;
						bufferSize < sizeof(buffer) && ch != EOF;
						++bufferSize, ch = getc(file)) {
					if (!isalnum(ch))
						lastSeparator = bufferSize;
					buffer[bufferSize] = ch;
				}

				printf("%u bytes readed\n", bufferSize);
				fillReverseIndex(indexTable, buffer, name);
			}
		}

		printf("do you want insert a file? ");
		getchar();
		op = getchar();
	}

	while (1) {
		printf("type key to search: ");
		char key[20];
		scanf("%s", key);
		showReverseIndex(indexTable, key);
	}
}


int main() {
	testFillReverseIndexInputed();
}
