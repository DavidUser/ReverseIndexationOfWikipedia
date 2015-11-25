#define COLISION_TEST
void colideOccurred() {
	printf("colision!\n");
}

#include "HashTable.h"
#include "hash.h"
#include <stdio.h>

void printAge(int * age);

main() {
	HashTable * table = newHashTable(5, JSHash);

	printf("inserting on hash table ...\n");
	int davidAge, ericaAge, adamAge, markAge, elonAge;
 	davidAge = 23;
	insertElementOnHashTable(table, "david", davidAge);
	ericaAge = 21;
	insertElementOnHashTable(table, "erica", ericaAge);
	adamAge = 19;
	insertElementOnHashTable(table, "adam", adamAge);
	markAge = 31;
	insertElementOnHashTable(table, "mark zuckerberg", markAge);
	elonAge = 44;
	insertElementOnHashTable(table, "elon musk", elonAge);

	printf("searching on hash table ...\n");
       	printAge(searchElementOnHashTable(table, "adam"));
       	printAge(searchElementOnHashTable(table, "erica"));
       	printAge(searchElementOnHashTable(table, "david"));
       	printAge(searchElementOnHashTable(table, "mark zuckerberg"));
       	printAge(searchElementOnHashTable(table, "elon musk"));
}

void printAge(int * age) {
	if (age)
		printf("age: %u\n", *age);
	else
		printf("not found!\n");
}
