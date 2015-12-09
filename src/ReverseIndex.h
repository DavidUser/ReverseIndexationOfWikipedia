#pragma once

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "LinkedList.h"
#include "DocumentOccurrence.h"
#include "time.h" // TODO only to test

#ifdef STRUCTURE_HashTable
#include "HashTable.h"
#endif
#ifdef STRUCTURE_Trie
#include "Trie.h"
#endif

#define KEY_SIZE 20

/* função de comparação que diz qual occorrência deve preceder em uma lista de ocorrência para dada chave
   occurrence		ocorrência que será inserida na lista
   occurrenceOnList	ocorrência pertence à lista

   retorna um valor diferente de zero se occurrence preceder occurrenceOnList
   */
int frequentDocumentOccurrence(void * occurrence, void * occurrenceOnList) {
	int result = strcmp(((DocumentOccurrence *) occurrence)->doc_id, ((DocumentOccurrence *) occurrenceOnList)->doc_id);
	return result > 0;
}
/* função que formata a chave para específicos 20 caracteres cujo menores tamanhos são preenchidos com vazios (caracter nulo)
   key	chave a ser formatada

   retorna a chave formatada
   */
const char * formatKey(const char * key) {
	char *validKey = malloc(20);
	int i;
	for (i = 0; key[i] != '\0'; ++i)
		validKey[i] = key[i];
	for (; i < 20; ++i)
		validKey[i] = 0;
	return validKey;
}

struct ReverseIndex {
	//TODO need contain the internal structure
	//TODO need contain the relevant function pointers
	size_t size;
} reverseIndex;

/* insere uma ocorrência na tabela de indices invertidos
   indexTable	tabela hash de indices invertidos
   key		chave, palavra indexada
   occurrence	ocorrência a ser inserida
   */
void insertDocumentOccurrence(STRUCTURE * indexTable, const char * key, DocumentOccurrence * occurrence) {
	key = formatKey(key);
#ifdef STRUCTURE_HashTable
	LinkedList * list = searchElementOnHashTable(indexTable, key);
	if (!list) {
		list = newLinkedList();
		insertStructElementOnHashTable(indexTable, key, list, 0);
	}
	//TODO replace by a hash instead a list
	pushBackStructElement(list, occurrence, sizeof(DocumentOccurrence));
#endif
#ifdef STRUCTURE_Trie
	Trie * list = newTrie();
	Trie * existentList = tryInsertStructElementOnTrie(indexTable, key, list, 0);
	if (existentList) {
		deleteTrie(list);
		list = existentList;
	}
	tryInsertStructElementOnTrie(list, occurrence->doc_id, occurrence, 0);
#endif
	++(reverseIndex.size); //TODO the reverse index need save the total ammount of document occurrences indexed
}
/* pega a lista de ocorrências para uma determinada palavra
   indexTable	tabela de indices invertidos
   key		palavra a ser buscada

   retorna a lista de ocorrência para a palavra buscada no indice
   */
STRUCTURE * getDocumentOccurrence(STRUCTURE * indexTable, const char * key) {
	key = formatKey(key);
#ifdef STRUCTURE_HashTable
	return searchElementOnHashTable(indexTable, key);
#endif
#ifdef STRUCTURE_Trie
	return searchElementOnTrie(indexTable, key);
#endif
}

LinkedList * getDocumentOccurrenceList(STRUCTURE * indexTable, const char * key) {
#ifdef STRUCTURE_Trie
	return getAllElements(getDocumentOccurrence(indexTable, key));
#endif
}

/* compara um identificador de documento a uma occorrência
   doc_id		identificador
   occurrenceOnList	ocorrência que pode referir a um documento

   retorna um valor diferente de zero se o identificador passado corresponde a ocorrência
   */
int occurrenceEqual(void * doc_id, void * occurrenceOnList) {
	return strcmp((char *) doc_id, ((DocumentOccurrence *) occurrenceOnList)->doc_id) == 0;
}

/* comparação entre ocorrências que determina qual vem primeiro na ordeção
   value	occorrência sendo inserida
   valueOnList	ocorrência já pertencente a lista

   retorna um valor diferente de zero se value é a precedente
   */
int occurrenceGreater(void * value, void * valueOnList) {
	return strcmp(((DocumentOccurrence *) value)->doc_id, ((DocumentOccurrence *) valueOnList)->doc_id) == 0;
}

/* preenche uma estrutura de indices invertidos partindo de uma string
   indexTable	tabela hash de indices invertidos
   str		string a ser compilada
   doc_id	identificador do documento
   */
double timeSearchingOnList = 0; // TODO only to test
void fillStructure(STRUCTURE * indexTable, const char * str, const char * doc_id) {
	// get words and quantities from str
	while (*str) {
		if (!isalnum(*str)) {
			++str;
			continue;
		}

		size_t word_i = 0;
		char word[20];
		for (; word_i < 20 && isalnum(*str); ++str, ++word_i)
			word[word_i] = (char) tolower(*str);
		while (word_i < 20)
			word[word_i++] = '\0';

	// procurar esta palavra no indice invertido
		Trie * occurrenceList = getDocumentOccurrence(indexTable, word);
	// se encontrar lista com essa entrada percorrer a lista buscando o doc_id
		if (occurrenceList) {
			time_t startTime = time(NULL); // TODO only to test
			// TODO replace linked list search by trie search
			DocumentOccurrence * found = searchElementOnTrie(occurrenceList, doc_id);
			timeSearchingOnList += difftime(time(NULL), startTime); // TODO only to test
	// se encontrar o doc_id somar a occorrencia das palavras
			if (found)
				++found->count;
	// se nao encontrar o doc_id adicionar entrada na lista com o doc_id e a quantidade
			else
				insertDocumentOccurrence(indexTable, word, 
						newDocumentOccurrence(doc_id, 1));
	// se nao encontrar entrada da palavra inserir no indice uma entrada com o doc_id e a quantidade
		} else
			insertDocumentOccurrence(indexTable, word,
					newDocumentOccurrence(doc_id, 1));
	}
	printf("time elapsed searching on list: %f\n", timeSearchingOnList); // TODO only to test
}

/* Calcula o peso de cada documento para um termo o qual a lista de ocorrencias é parametro
   indexTable	tablela de indices
   occurrences	lista com as ocorrências do termo nos documentos

   retorna um vetor com os pesos correspondentes de cada documento
   */
double * weighsOccurrences(LinkedList * occurrences) {
	double * weights = calloc(occurrences->size, sizeof(double));
	size_t weightsIndex = 0;

	LinkedListIterator * iterator = newLinkedListIterator(occurrences);
	while (hasNext(iterator)) {
		DocumentOccurrence * occurrence = (DocumentOccurrence *) getValue(iterator);
		weights[weightsIndex++] = occurrence->count * (log((double) reverseIndex.size) / occurrences->size);
	}
	free(iterator);

	return weights;
}
