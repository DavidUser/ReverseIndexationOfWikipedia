#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#define STRUCTURE HashTable
#define STRUCTURE_HashTable
// #define STRUCTURE Trie
// #define STRUCTURE_Trie

#include "ReverseIndex.h"
#include "hash.h"

/*
   Função hash simplista

   key	chave tipo string para se calcular o hash
   length comprimento da chave

   retorna o valor hash
  */
size_t hash(const char *key, size_t length) {
	int hashCode = 0;
	for (size_t i = 0; i < length; ++i)
		hashCode += key[i];
	return hashCode;
}

/* Função que mostra no console as entradas na tabela hash para uma determinada consulta,
   o termo buscado serve para calcular o peso de cada documento
   indexTable	tabela com os indices invertidos
   key		chave do tipo string para consulta
*/
void showReverseIndex(STRUCTURE * indexTable, const char *key) {
	printf("%s: ", key);
	char * keyCopy = strdup(key);
	size_t numberOfTerms = 1;
	for (size_t i = 0; key[i] != '\0'; ++i) if (key[i] == ' ') ++numberOfTerms;
	char ** keyTerm = calloc(numberOfTerms, sizeof(char *));
	size_t iKeyTerm = 0;
	for (char * part = strtok(keyCopy, " ");
			part; part = strtok(NULL, " "))
		keyTerm[iKeyTerm++] = strdup(part);
	LinkedList ** occurrenceList = calloc(numberOfTerms, sizeof(LinkedList *));
	LinkedList * occurrenceListUnified = newLinkedList(); //a unified list, with occurrence of all search terms
	for (size_t i = 0; i < numberOfTerms; ++i) {
		//XXX get document occurrence return STRUCTURE type, not a linked list
		occurrenceList[i] = getDocumentOccurrenceList(indexTable, keyTerm[i]); //TODO convert Trie to list
		if (occurrenceList[i])
			//for all document occurrences
			for (LinkedListIterator * iterator = newLinkedListIterator(occurrenceList[i]);
					hasNext(iterator);) {
				DocumentOccurrence * documentOccurrence = getValue(iterator);
				DocumentOccurrence * occurrenceOnUnified = searchElement(occurrenceListUnified, documentOccurrence->doc_id, occurrenceEqual);
				//if term found on unifiedList increase the occurrences frequence
				if (occurrenceOnUnified)
					occurrenceOnUnified->count += documentOccurrence->count;
				//else insert a new DocumentOccurrece on unifiedList with this frequence found
				else
					pushBackStructElement(occurrenceListUnified, documentOccurrence, sizeof(DocumentOccurrence));
			}
	}
	if (occurrenceListUnified->size) {
		//calcula os pesos de cada documento, documentos sem o termo tem peso 0 e não precisam ser computados
		double ** weights = calloc(numberOfTerms, sizeof(double *));
		for (size_t i = 0; i < numberOfTerms; ++i)
	       		weights[i] = weighsOccurrences(occurrenceList[i]);
		//calcula a relevancia de um documento
		double * relevance = calloc(occurrenceListUnified->size, sizeof(double));
		// for all documents on unifiedList
		int iRelevance = 0; 
		for (LinkedListIterator * iteratorUnified = newLinkedListIterator(occurrenceListUnified);
				hasNext(iteratorUnified); ++iRelevance) {
			DocumentOccurrence * occurrenceUnified = getValue(iteratorUnified);
			// found the document weigthts for each term 
			// sum all weights relative to a document and
			// put this sum on relevance
			relevance[iRelevance] = 0;
			for (size_t i = 0; i < numberOfTerms; ++i) {
				size_t iFoundDocument = 0;
				for (LinkedListIterator * iterator = newLinkedListIterator(occurrenceList[i]);
						hasNext(iterator); ++iFoundDocument) {
					DocumentOccurrence * occurrence = getValue(iterator);
					if (strcmp(occurrence->doc_id, occurrenceUnified->doc_id)) break;
				}
				if (iFoundDocument != occurrenceList[i]->size)
					relevance[iRelevance] += weights[i][iFoundDocument];
			}
		}

		size_t * rank = calloc(occurrenceListUnified->size, sizeof(size_t));
		for (int i = 0; i < occurrenceListUnified->size; ++i)
			for (int j = 0; j < occurrenceListUnified->size; ++j)
				if (relevance[i] < relevance[j] || (relevance[i] == relevance[j] && i < j))
					++rank[i];
		
		// constroi um vetor de ponteiros ordenado pela relevancia de cada documento
		DocumentOccurrence ** occurrencesSorted = calloc(occurrenceListUnified->size, sizeof(DocumentOccurrence *));
		int sortedIndex = 0;
		LinkedListIterator * it = newLinkedListIterator(occurrenceListUnified);
		while (hasNext(it)) {
			DocumentOccurrence * occurrence = (DocumentOccurrence *) getValue(it);
			occurrencesSorted[rank[sortedIndex++]] = occurrence;
		}

		// percorre o vetor ordenado pela relevancia imprimindo no console
		for (int i = 0; i < occurrenceListUnified->size; ++i)
			printf("<%u, %s> ", occurrencesSorted[i]->count, occurrencesSorted[i]->doc_id);
	}
	printf("\n\n");
}

/* Testa a estrutura básica da tabela hash de indices invertidos */
void testBasicStructure() {
	STRUCTURE * indexTable = 
#ifdef STRUCTURE_HashTable
		newHashTable(60, hash);
#endif
#ifdef STRUCTURE_Trie
		newTrie();
#endif
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

/* Testa a compilação de strings para o preenchimento da tabela hash de indices invertidos */
void testFillStructure() {
	STRUCTURE * indexTable = 
#ifdef STRUCTURE_HashTable
		newHashTable(60, hash);
#endif
#ifdef STRUCTURE_Trie
		newTrie();
#endif
	fillStructure(indexTable, "cat dog cat", "doc1");
	fillStructure(indexTable, "dog dog dog", "doc1");
	fillStructure(indexTable, "dog dog dog", "doc2");
	fillStructure(indexTable, "dog dog cat", "doc2");

	showReverseIndex(indexTable, "cat");
	showReverseIndex(indexTable, "dog");
	showReverseIndex(indexTable, "bird");
}

/* Substitui acentos e demais caracteres especiais
   wc	caractere que pode conter acentos e outros caracteres especiais

   retorna uma versão ASC compatível para o caractere
   */
const char wcharToChar(wchar_t wc) {
	const wchar_t wide[] =  	L"àâêôûãõáéíóúçüÀÂÊÔÛÃÕÁÉÍÓÚÇÜ";
	const char asc[] =		"aaeouaoaeioucuAAEOUAOAEIOUCU";

	for (size_t i = 0; i < sizeof(asc); ++i)
		if (wc == wide[i])
			return asc[i];
	return (char) wc;
}

/* Testa a compilação de arquivos para o preenchimento da tabela hash de indices invertidos */
void testArchiveFillStructure() {
	STRUCTURE * indexTable = 
#ifdef STRUCTURE_HashTable
		newHashTable(60, hash);
#endif
#ifdef STRUCTURE_Trie
		newTrie();
#endif

	printf("do you want insert a file? ");
	char op = getchar();
	while (op == 'y') {
		char name[20];
		printf("file name: ");
		scanf("%s", name);
		FILE * file = fopen(name, "r");
		if (file) {
			while (!feof(file)) {
				char buffer[500];
				size_t bufferSize = 0;
				size_t lastSeparator = 0;
				for (char ch = wcharToChar(getwc(file)) ;
						bufferSize < sizeof(buffer) && ch != EOF;
						++bufferSize, ch = getc(file)) {
					if (!isalnum(ch))
						lastSeparator = bufferSize;
					buffer[bufferSize] = ch;
				}

				//XXX partial words can be lost, if the word are between this buffer and another
				// 500 character size buffer works to short abstracts without problem
				printf("%u bytes readed\n", bufferSize);
				fillStructure(indexTable, buffer, name);
			}
		}

		printf("do you want insert a file? ");
		getchar();
		op = getchar();
	}

	char key[20];
	do {
		printf("type key to search: ");
		fgets(key, 20, stdin);
		showReverseIndex(indexTable, key);
	} while (key[0] != '*');
}

void testReadShortAbstracts() {
	//XXX low capacity can cause much hash colisions
	FILE * file;
	for (file = NULL; !file; ) {
		char name[255];
		printf("file name: ");
		scanf("%s", name);
		file = fopen(name, "r");
	}
	
	fseek(file, 0L, SEEK_END);
	size_t totalFileSize = ftell(file);
	fseek(file, 0L, SEEK_SET);
	size_t totalReaded = 0;

#ifdef STRUCTURE_HashTable
	printf(
			"0 - Robert Sedgwicks\n"
			"1 - Justin Sobel\n"
			"2 - Brian Kernighan and Dennis Ritchie's\n"
			"3 - Daniel J. Bernstein\n"
			"4 - ELF\n"
			"Select hash function (0-4): " 
	      );

	HashTable * indexTable; 

	int selectHash = 1;
	while (selectHash) {
		selectHash = 0;
		int op;
		scanf("%d", &op);
		switch (op) {
			case 0:
				indexTable = newHashTable(totalFileSize/20, RSHash);
				break;
			case 1:
				indexTable = newHashTable(totalFileSize/20, JSHash);
				break;
			case 2:
				indexTable = newHashTable(totalFileSize/20, BKDRHash);
				break;
			case 3:
				indexTable = newHashTable(totalFileSize/20, DJBHash);
				break;
			case 4:
				indexTable = newHashTable(totalFileSize/20, ELFHash);
				break;
			default:
				printf("Ivalid selection type a number between 0 and 4!\n");
				selectHash = 1;
		}
	}

#endif
#ifdef STRUCTURE_Trie
	Trie * indexTable = newTrie();
#endif
	if (file) {
		while (!feof(file)) {
			char *entrie = NULL;
			size_t bufferSize = 0;
			getline(&entrie, &bufferSize, file);

			if (entrie[0] != '<') { // if it is't a entrie
				free(entrie);
				continue;
			}
			// get doc id
			char doc_id[500];
			size_t doc_idSize = 0;
			int capture = 0;
			for (size_t i = 0; i < bufferSize; ++i) {
				if (entrie[i] == '<')
					capture = 1;
				else if (entrie[i] == '>') {
					doc_id[doc_idSize++] = '\0';
					break;
				} else if (capture)
					doc_id[doc_idSize++] = entrie[i];
			}

			double percent = (((double) totalReaded) / totalFileSize) * 100;
			totalReaded += bufferSize;
			printf("%lf %% --  %lu bytes readed of %lu\n", percent, totalReaded, totalFileSize);
			fillStructure(indexTable, entrie, doc_id);
			free(entrie);
		}
	}

	fclose(file);


	getchar();

	char key[20];
	do {
		printf("type key to search: ");
		fgets(key, 20, stdin);
		key[strcspn(key, "\n")] = '\0';
		showReverseIndex(indexTable, key);
	} while (key[0] != '*');

	//TODO delete when Hash table
#ifdef STRUCTURE_Trie
	//deleteTrie(indexTable);
#endif
}

int main() {
	//testBasicStructure();
	//testFillStructure();
	//testFillStructureInputed(); // a função principal testando o preenchimento da tabela de indices invertidos por meio de arquivos
	testReadShortAbstracts(); // cria indice invertido partido de shortabstracts
}
