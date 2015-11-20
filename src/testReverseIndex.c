#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include "HashTable.h"
#include "hash.h"

#define STRUCTURE HashTable
#define STRUCTURE_HashTable

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
#ifdef STRUCTURE_HashTable
	printf("%s: ", key);
	LinkedList * occurrenceList = getDocumentOccurrence(indexTable, key);
	if (occurrenceList) {
		//calcula os pesos de cada documento, documentos sem o termo tem peso 0 e não precisam ser computados
		double * weigthts = weighsOccurrences(indexTable, occurrenceList);
		size_t * rank = calloc(occurrenceList->size, sizeof(size_t));
		for (int i = 0; i < occurrenceList->size; ++i)
			for (int j = 0; j < occurrenceList->size; ++j)
				if (weigthts[i] < weigthts[j] || (weigthts[i] == weigthts[j] && i < j))
					++rank[i];
		
		// constroi um vetor de ponteiros ordenado pela relevancia de cada documento
		DocumentOccurrence ** occurrencesSorted = calloc(occurrenceList->size, sizeof(DocumentOccurrence *));
		int sortedIndex = 0;
		LinkedListIterator * it = newLinkedListIterator(occurrenceList);
		while (hasNext(it)) {
			DocumentOccurrence * occurrence = (DocumentOccurrence *) getValue(it);
			occurrencesSorted[rank[sortedIndex++]] = occurrence;
		}

		// percorre o vetor ordenado pela relevancia imprimindo no console
		for (int i = 0; i < occurrenceList->size; ++i)
			printf("<%u, %s> ", occurrencesSorted[i]->count, occurrencesSorted[i]->doc_id);
	}
	printf("\n");
#endif
}

/* Testa a estrutura básica da tabela hash de indices invertidos */
void testBasicStructure() {
#ifdef STRUCTURE_HashTable
	HashTable * indexTable = newHashTable(60, hash);
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
#endif
}

/* Testa a compilação de strings para o preenchimento da tabela hash de indices invertidos */
void testFillHashTable() {
#ifdef STRUCTURE_HashTable
	HashTable * indexTable = newHashTable(60, hash);
	fillHashTable(indexTable, "cat dog cat", "doc1");
	fillHashTable(indexTable, "dog dog dog", "doc1");
	fillHashTable(indexTable, "dog dog dog", "doc2");
	fillHashTable(indexTable, "dog dog cat", "doc2");

	showReverseIndex(indexTable, "cat");
	showReverseIndex(indexTable, "dog");
	showReverseIndex(indexTable, "bird");
#endif
}

/* Substitui acentos e demais caracteres especiais
   wc	caractere que pode conter acentos e outros caracteres especiais

   retorna uma versão ASC compatível para o caractere
   */
char wcharToChar(wint_t wc) {
	wchar_t *wide = "àâêôûãõáéíóúçüÀÂÊÔÛÃÕÁÉÍÓÚÇÜ";
	char *asc =	 "aaeouaoaeioucuAAEOUAOAEIOUCU";

	for (size_t i = 0; i < sizeof(wide); ++i)
		if (wc == wide[i])
			return asc[i];
}

/* Testa a compilação de arquivos para o preenchimento da tabela hash de indices invertidos */
void testFillHashTableInputed() {
#ifdef STRUCTURE_HashTable
	HashTable * indexTable = newHashTable(60, hash);
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
#ifdef STRUCTURE_HashTable
				fillHashTable(indexTable, buffer, name);
#endif
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

	printf(
			"0 - Robert Sedgwicks\n"
			"1 - Justin Sobel\n"
			"2 - Brian Kernighan and Dennis Ritchie's\n"
			"3 - Daniel J. Bernstein\n"
			"4 - ELF\n"
			"Select hash function (0-4): " 
	      );

#ifdef STRUCTURE_HashTable
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
	if (file) {
		while (!feof(file)) {
			char *entrie = NULL;
			size_t bufferSize = 0;
			getline(&entrie, &bufferSize, file);

			if (entrie[0] != '<') // if it is't a entrie
				continue;
			// get doc id
			char doc_id[500];
			size_t doc_idSize = 0;
			int capture = 0;
			for (size_t i = 0; i < bufferSize; ++i) {
				if (capture)
					doc_id[doc_idSize++] = entrie[i];
				if (entrie[i] == '<')
					capture = 1;
				else if (entrie[i] == '>')
					break;
			}

			double percent = (((double) totalReaded) / totalFileSize) * 100;
			totalReaded += bufferSize;
			printf("%lf %% --  %lu bytes readed of %lu\n", percent, totalReaded, totalFileSize);

#ifdef STRUCTURE_HashTable
			fillHashTable(indexTable, entrie, doc_id);
#endif
		}
	}

	fclose(file);

	while (1) {
		printf("type key to search: ");
		char key[20];
		scanf("%s", key);
		showReverseIndex(indexTable, key);
	}
}

int main() {
	//testFillHashTableInputed(); // a função principal testando o preenchimento da tabela de indices invertidos por meio de arquivos
	testReadShortAbstracts(); // cria indice invertido partido de shortabstracts
}
