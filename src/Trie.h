#pragma once

#include <stdlib.h>
#include "LinkedList.h"

typedef struct TrieNode TrieNode;

typedef struct Trie Trie;
struct Trie {
	TrieNode * root;
	size_t size;
	size_t height;
	size_t ammountOfNodes;
};

struct TrieNode {
	char partialKey;
	LinkedList * postfixes;
	void * content;
};

TrieNode * newTrieNode(char value) {
	TrieNode * node = malloc(sizeof(TrieNode));
	node->partialKey = value;
	node->postfixes = newLinkedList();
	node->content = NULL;
	return node;
}
void deleteTrieNode(TrieNode * node) {
	free(node->content);
	free(node);
}

Trie * newTrie() {
	Trie * trie = malloc(sizeof(Trie));
	trie->root = newTrieNode('\0');
	trie->size = 0;
	trie->height = 0;
	trie->ammountOfNodes = 0;
	return trie;
}
void deleteTrie(Trie * trie) {
	//TODO iterate between the TRIE deleting all nodes
	free(trie);
}

int characterOnTrieNode(void * character, void * trieNodeOnList) {
	return *((char *) character) == ((TrieNode *) trieNodeOnList)->partialKey;
}

/* Insere uma entrada na arvore
   trie		arvore
   key		chave
   value	valor associado a chave
   valueSize	tamanho da estrutura associada a chave

   retorna 0 se conseguiu inserir ou o valor inserido anteriormente para aquela chave
   */
void * tryInsertStructElementOnTrie(Trie * trie, const char * key, void * value, size_t valueSize) {
	TrieNode * current = trie->root;
	for (size_t i = 0; key[i] != '\0'; ++i) {
		TrieNode * found = searchElement(current->postfixes, &key[i], characterOnTrieNode);
		if (!found) {
			found = newTrieNode(key[i]);
			pushBackStructElement(current->postfixes, found, sizeof(TrieNode));
			++trie->ammountOfNodes;
			if (i > trie->height)
				++trie->height;
		}
		current = found;
	}
	const char endChar = '\0';
	TrieNode * node = searchElement(current->postfixes, &endChar, characterOnTrieNode);
	if (node)
		return node->content;//the key already exist on TRIE

	// put the content on last character (end character, null character)
	node = malloc(sizeof(TrieNode));
	node->partialKey = endChar;
	if (valueSize) {
		node->content = malloc(valueSize);
		memcpy(node->content, value, valueSize);
	} else
		node->content = value;
	pushBackStructElement(current->postfixes, node, sizeof(TrieNode));
	++trie->ammountOfNodes;
	++trie->size;
	return 0; // sucessful insertion
}
#define tryInsertElementOnTrie(TRIE, KEY, CONTENT) tryInsertStructElementOnTrie(TRIE, KEY, &CONTENT, sizeof(CONTENT))

/* Busca um elemento na TRIE
   trie árvore
   key	chave

   retorna o valor associado a chave ou NULL caso não encontre valor para a respectiva chave
   */
void * searchElementOnTrie(Trie * trie, const char * key) {
	TrieNode * current = trie->root;
	for (size_t i = 0; key[i] != '\0'; ++i) {
		TrieNode * found = searchElement(current->postfixes, &key[i], characterOnTrieNode);
		if (!found) return NULL;
		current = found;
	}
	const char endChar = '\0';
	TrieNode * node =  searchElement(current->postfixes, &endChar, characterOnTrieNode);
	return node? node->content : NULL;
}
