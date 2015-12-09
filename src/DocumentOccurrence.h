#pragma once

#include <string.h>
#include <stdlib.h>

typedef struct DocumentOccurrence DocumentOccurrence;
struct DocumentOccurrence {
	char * doc_id;
	size_t count;
};

#define DOC_ID_SIZE 200

/* cria uma instância para a estrutura que representa a ocorrência de uma chave em um documento
   doc_id	identificador do documento
   count	número de ocorrências no documento

   retorna a instância
   */
DocumentOccurrence * newDocumentOccurrence(const char * doc_id, const size_t count) {
	DocumentOccurrence * occurrence = malloc(sizeof(DocumentOccurrence));
	occurrence->doc_id = strndup(doc_id, DOC_ID_SIZE);
	occurrence->count = count;
	return occurrence;
}
/* deleta a instância da estrutura que representa a ocorrência de uma palavra no documento liberando a memórica
   occurrence	instância
   */
void deleteDocumentOccurrence(DocumentOccurrence * occurrence) {
	free(occurrence);
}
