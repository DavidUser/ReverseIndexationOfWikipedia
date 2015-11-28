#pragma once

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "LinkedList.h"
#include "DocumentOccurrence.h"
#include "HashTable.h"
#include <math.h>

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
	insertStructElementOrdered(list, occurrence, sizeof(DocumentOccurrence), frequentDocumentOccurrence, atRight);
	++(reverseIndex.size); //TODO the reverse index need save the total ammount of document occurrences indexed
#endif
}
/* pega a lista de ocorrências para uma determinada palavra
   indexTable	tabela de indices invertidos
   key		palavra a ser buscada

   retorna a lista de ocorrência para a palavra buscada no indice
   */
LinkedList * getDocumentOccurrence(STRUCTURE * indexTable, const char * key) {
	key = formatKey(key);
#ifdef STRUCTURE_HashTable
	return (LinkedList *) searchElementOnHashTable(indexTable, key);
#endif
}

typedef struct WordFrequence WordFrequence;
struct WordFrequence {
	const char * word;
	size_t count;
};

/* cria uma nova instância de estrutura que define o par palavra e número de ocorrências
   word		palavra encontrada
   count	número de vezes que a palavra foi encontrada

   retorna instância
   */
WordFrequence * newWordFrequence(const char *word, size_t count) {
	WordFrequence *wordFrequence = malloc(sizeof(WordFrequence));
	wordFrequence->word = malloc(20);
	strcpy(wordFrequence->word, word);
	wordFrequence->count = count;

	return wordFrequence;
}

/* compara uma palavra a palavra contida em uma estrutura de frequencia
   value	palavra verificada
   valueOnList	estrutura que contem palavra e frequência

   retorna um valor diferente de 0 se a palavra corresponde a estrutura
   */
int wordFrequenceEqual(void * value, void * valueOnList) {
	return strcmp((char *) value, ((WordFrequence *) valueOnList)->word) == 0;
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

/* preenche uma tabela hash de indices invertidos partindo de uma string
   indexTable	tabela hash de indices invertidos
   str		string a ser compilada
   doc_id	identificador do documento
   */
void fillHashTable(STRUCTURE * indexTable, const char * str, const char * doc_id) {
#ifdef STRUCTURE_HashTable
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
		WordFrequence * wordFrequence = (WordFrequence *) getValue(iterator);

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
#endif
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

	return weights;
}
