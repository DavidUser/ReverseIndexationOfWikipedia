#include "Trie.h"
#include <stdio.h>

main() {
	Trie * trie = newTrie();
	int davidAge = 23;
	int danielAge = 25;
	int ericaAge = 21;
	int stherAge = 15;
	tryInsertElementOnTrie(trie, "david", davidAge);
	tryInsertElementOnTrie(trie, "daniel", danielAge);
	tryInsertElementOnTrie(trie, "erica", ericaAge);
	tryInsertElementOnTrie(trie, "sther", stherAge);
	
	char name[20];
	do {
		printf("Type the name to get the age: ");
		scanf("%s", name);
		int * age = searchElementOnTrie(trie, name);
		if (age)
			printf("%d\n", *age);
		else
			printf("age not found!\n");
	} while (name[0] != '*');
	deleteTrie(trie);
}
