#pragma once

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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
	int result = strcmp(((DocumentOccurrence *) occurrence)->doc_id, ((DocumentOccurrence *) occurrenceOnList)->doc_id);
	return result > 0;
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
	insertStructElementOrdered(indexTable->occurenceLists[index], occurrence, sizeof(DocumentOccurrence), frequentDocumentOccurrence, atRight);
}
LinkedList * getDocumentOccurrence(ReverseIndex * indexTable, const char * key) {
	key = formatKey(key);
	size_t index = indexTable->hash(key, 20) % indexTable->capacity;
	return indexTable->occurenceLists[index];
}

typedef struct WordFrequence WordFrequence;
struct WordFrequence {
	const char * word;
	size_t count;
};

WordFrequence * newWordFrequence(const char *word, size_t count) {
	WordFrequence *wordFrequence = malloc(sizeof(WordFrequence));
	wordFrequence->word = malloc(20);
	strcpy(wordFrequence->word, word);
	wordFrequence->count = count;

	return wordFrequence;
}

int wordFrequenceEqual(void * value, void * valueOnList) {
	return strcmp((char *) value, ((WordFrequence *) valueOnList)->word) == 0;
}

int occurrenceEqual(void * doc_id, void * occurrenceOnList) {
	return strcmp((char *) doc_id, ((DocumentOccurrence *) occurrenceOnList)->doc_id) == 0;
}

int occurrenceGreater(void * value, void * valueOnList) {
	return ((DocumentOccurrence *) value)->count > ((DocumentOccurrence *) valueOnList)->count;
}

void fillReverseIndex(ReverseIndex * indexTable, const char * str, const char * doc_id) {
	LinkedList * words = newLinkedList();

	// get words and quantities from str
	while (*str) {
		if (!isalnum(*str)) {
			++str;
			continue;
		}

		size_t word_i = 0;
		char * word = malloc(20);
		for (; word_i < 20 && isalnum(*str); ++str, ++word_i)
			word[word_i] = (char) tolower(*str);
		while (word_i < 20)
			word[word_i++] = '\0';

		WordFrequence * found = ((WordFrequence *) searchElement(words, word, wordFrequenceEqual));
		if (found)
			++(found->count);
		else
			pushBackStructElement(words, newWordFrequence(word, 1), sizeof(WordFrequence));
	}

	LinkedListIterator * iterator = newLinkedListIterator(words);
	while(hasNext(iterator)) {
		WordFrequence * wordFrequence = getValue(iterator);

	// procurar estas palavras no indice invertido
		LinkedList * occurrenceList = getDocumentOccurrence(indexTable, wordFrequence->word);
	// se encontrar lista com essa entrada percorrer a lista buscando o doc_id
		if (occurrenceList) {
			DocumentOccurrence * found = searchElement(occurrenceList, doc_id, occurrenceEqual);
	// se encontrar o doc_id somar a occorrencia das palavras
			if (found)
				found->count += wordFrequence->count;
	// se nao encontrar o doc_id adicionar entrada na lista com o doc_id e a quantidade
			else
				insertDocumentOccurrence(indexTable, wordFrequence->word,
					newDocumentOccurrence(doc_id, wordFrequence->count));
	// se nao entrada da palavra inserir no indice uma entrada com o doc_id e a quantidade
		} else
			insertDocumentOccurrence(indexTable, wordFrequence->word,
					newDocumentOccurrence(doc_id, wordFrequence->count));
	}
}
