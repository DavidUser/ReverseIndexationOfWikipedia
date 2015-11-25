#pragma once

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "DocumentOccurrence.h"
#include "LinkedList.h"
#include <math.h>

typedef struct HashTable HashTable;
typedef size_t (* HashFunction)(const char * key, size_t lenth);
struct HashTable {
	void ** values;
	const char ** keys;
	size_t size;
	size_t capacity;
	HashFunction hash;
};

#define KEY_SIZE 20

/* cria instância para estrutura que representa a tabela hash de indices invertidos
   entries	capacidade da tabela hash
   hash		função hash utilizada pela estrutura

   retorna a instância
   */
HashTable * newHashTable(size_t entries, HashFunction hash) {
	HashTable * indexTable = malloc(sizeof(HashTable));
	indexTable->values = calloc(entries, sizeof(void *));
	indexTable->keys = calloc(entries, sizeof(const char *));
	for (size_t i = 0; i < entries; ++i)
		indexTable->values[i] = NULL;
	indexTable->size = 0;
	indexTable->capacity = entries;
	indexTable->hash = hash;
	return indexTable;
}
/* desaloca memória para a tabela de indices invertidos
   indexTable	tabela hash de indices invertidos
   */
void deleteHashTable(HashTable * indexTable) {
//	for (size_t i = 0; i < indexTable->capacity; ++i)
//		if (indexTable->values[i])
			; // TODO function to delete all values allocated
	free(indexTable);
}
/* insere uma ocorrência na tabela de indices invertidos
   indexTable	tabela hash de indices invertidos
   key		chave, palavra indexada
   value	valor a ser inserido associado a chave
   sizeOfValue	tamanho do valor, se for zero indica que não deve ser feita cópia do valor passado
   */
void insertStructElementOnHashTable(HashTable * indexTable, const char * key, void * value, size_t sizeOfValue) {
	if (indexTable->size == indexTable->capacity) return; // XXX error, full hash table
	size_t index = indexTable->hash(key, 20) % indexTable->capacity;
colisionTreat:
	if (!indexTable->keys[index]) { // empty entrie
		indexTable->keys[index] = malloc(strlen(key)); 
		strcpy(indexTable->keys[index], key);
		if (sizeOfValue) { // copy because value is informed
			indexTable->values[index] = malloc(sizeOfValue);
			memcpy(indexTable->values[index], value, sizeOfValue);
		} else // just store the pointer
			indexTable->values[index] = value;
		++(indexTable->size);
	} else if (strcmp(indexTable->keys[index], key) != 0) { //XXX colision
#ifdef COLISION_TEST
		colideOccurred();
#endif
		//TODO treat colision
		do index = (index + 1) % indexTable->capacity;
		while (indexTable->keys[index]); // find a empty entry
		goto colisionTreat;
	}
}
#define insertElementOnHashTable(TABLE, KEY, VALUE) insertStructElementOnHashTable(TABLE, KEY, &VALUE, sizeof VALUE)
/* pega a lista de ocorrências para uma determinada palavra
   indexTable	tabela de indices invertidos
   key		palavra a ser buscada

   retorna a lista de ocorrência para a palavra buscada no indice
   */
void * searchElementOnHashTable(HashTable * indexTable, const char * key) {
	size_t index = indexTable->hash(key, 20) % indexTable->capacity;
	if (!indexTable->keys[index]) return NULL; // element not found
	if (strcmp(indexTable->keys[index], key) == 0)
		return indexTable->values[index];
	//XXX colision
#ifdef COLISION_TEST
	colideOccurred();
#endif
	//treating search colided
	for (size_t i = (index + 1) % indexTable->capacity; i != index; i = (i + 1) % indexTable->capacity)
		if (indexTable->keys[i] && strcmp(indexTable->keys[i], key) == 0)
			return indexTable->values[i];
	return NULL; // element not found
}
