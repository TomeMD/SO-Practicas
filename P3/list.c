#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ftw.h>
#include <grp.h>
#include <pwd.h>
#include <errno.h>
#include "list.h"

#define FALSE 0
#define TRUE 1

tNodo inicializarNodo(tNodo nodo) {
	nodo.addr = 0;
	nodo.size = 0;
	nodo.type = "";
	nodo.key = 0;
	nodo.df = 0;
	nodo.nameFile = "";
	strcpy(nodo.time, "");
	nodo.id = 0;
	
	return nodo;
}

tNodo crearNodoMalloc(char* trozos[], void* addr) {
	tNodo nodo;
	time_t tiempo = time(NULL);
	struct tm *timer = localtime(&tiempo);

	nodo.addr = addr;
	nodo.size = atoi(trozos[2]);
	nodo.type = "malloc";
	nodo.key = 0;
	nodo.nameFile = "";	
	nodo.df = 0;
	strftime(nodo.time, 50,"%a %b %d %H:%M:%S %Y", timer);
	nodo.id = 0;
	
	return nodo;
}

tNodo crearNodoMMap(char* file, void* addr, size_t size, int df) {
	tNodo nodo;
	time_t tiempo = time(NULL);
	struct tm *timer = localtime(&tiempo);
	
	nodo.addr = addr;
	nodo.size = size;
	nodo.type = "mmap";
	nodo.key = 0;
	nodo.nameFile = file;	
	nodo.df = df;
	strftime(nodo.time, 50,"%a %b %d %H:%M:%S %Y", timer);
	nodo.id = 0;
	
	return nodo;
}


tNodo crearNodoCreateShared(key_t key, size_t size, void* addr, int id) {
	tNodo nodo;
	time_t tiempo = time(NULL);
	struct tm *timer = localtime(&tiempo);
	
	nodo.addr = addr;
	nodo.size = size;
	nodo.type = "shared memory";
	nodo.key = key;
	nodo.nameFile = "";	
	nodo.df = 0;
	strftime(nodo.time, 50,"%a %b %d %H:%M:%S %Y", timer);
	nodo.id = id;
	
	return nodo;
}


int esListaVacia(tList lista) {
	return (lista.fin == -1);
}

void crearListaVacia(tList* lista) {
	
	for (int i = 0; i < MAXC; i++) lista->nodos[i] = NULL;
	lista->fin = -1;
}

void insertarNodo(tNodo* nodo, tList* lista) {
	
	if ((lista->fin + 1) > MAXC) {
		printf("Error: lista llena.\n");
	} else {
		lista->fin+=1;
		lista->nodos[lista->fin] = malloc (sizeof(tNodo));
		lista->nodos[lista->fin]->addr = nodo->addr;
		lista->nodos[lista->fin]->size = nodo->size;
		lista->nodos[lista->fin]->type = nodo->type;
		lista->nodos[lista->fin]->key = nodo->key;
		lista->nodos[lista->fin]->df = nodo->df;
		lista->nodos[lista->fin]->nameFile = nodo->nameFile;
		strcpy(lista->nodos[lista->fin]->time, nodo->time);
		lista->nodos[lista->fin]->id = nodo->id;
		nodo = NULL;
	}

}
void borrarNodo(int pos, tList* lista) {
		
	if (lista->nodos[pos] == NULL) {
		printf("Error: el nodo no existe.");
	} else {
		if (strcmp(lista->nodos[pos]->type, "malloc") == 0)
			free(lista->nodos[pos]->addr);
		free(lista->nodos[pos]);
		for (int i = pos; i < lista->fin; i++) {
			lista->nodos[i] = lista->nodos[i+1];
		}
		lista->nodos[lista->fin] = NULL;
		lista->fin--;
	}
		
}

int getPosicionSize(size_t size, tList lista) {
	int i = 0;
	
	while ((i <= lista.fin) && (lista.nodos[i] != NULL)) {
		if (lista.nodos[i]->size == size) break;
		i++;
	}
	if (i > lista.fin) i = -1;
	return i;
}

int getPosicionAddr(char* addr, tList lista) {
	int i = 0;
	unsigned long int address = strtoul(addr, NULL, 16);
	
	while ((i <= lista.fin) && (lista.nodos[i] != NULL)) {
		if ((unsigned long int) lista.nodos[i]->addr == address) break;
		i++;
	}
	if (i > lista.fin) i = -1;
	return i;
}


int getPosicionFich(char* nameFile, tList lista) {
	int i = 0;
	
	while ((i <= lista.fin) && (lista.nodos[i] != NULL)) {
		if (strcmp(lista.nodos[i]->nameFile, nameFile) == 0) break;
		i++;
	}
	if (i > lista.fin) i = -1;
	return i;
}

int getPosicionKey(key_t key, tList lista) {
	int i = 0;
	
	while ((i <= lista.fin) && (lista.nodos[i] != NULL)) {
		if (lista.nodos[i]->key == key) break;
		i++;
	}
	if (i > lista.fin) i = -1;
	return i;
}

void mostrarListaMalloc(tList lista) {
	int i = 0;
	
	if (!esListaVacia(lista)) {
		while ((i <= lista.fin) && (lista.nodos[i] != NULL)) {
			if (strcmp(lista.nodos[i]->type, "malloc") == 0) {
				printf("%p: ", lista.nodos[i]->addr);
				printf("size:%ld. ",lista.nodos[i]->size);
				printf("%s ", lista.nodos[i]->type);
				printf("%s\n", lista.nodos[i]->time);
			}
			i++;
		}
	}
	
}

void mostrarListaMMap(tList lista) {
	int i = 0;
	
	if (!esListaVacia(lista)){
	while ((i <= lista.fin) && (lista.nodos[i] != NULL)) {
		if (strcmp(lista.nodos[i]->type, "mmap") == 0) {
			printf("%p: ", lista.nodos[i]->addr);
			printf("size:%ld. ",lista.nodos[i]->size);
			printf("%s ", lista.nodos[i]->type);
			printf("%s ", lista.nodos[i]->nameFile);
			printf("(df:%d) ", lista.nodos[i]->df);
			printf("%s\n", lista.nodos[i]->time);
		}
		i++;
	}
}
	
}

void mostrarListaShared(tList lista) {
	int i = 0;
	
	while ((i <= lista.fin) && (lista.nodos[i] != NULL)) {
		if (strcmp(lista.nodos[i]->type, "shared memory") == 0) {
			printf("%p: ", lista.nodos[i]->addr);
			printf("size:%ld. ",lista.nodos[i]->size);
			printf("%s ", lista.nodos[i]->type);
			printf("(key %d) ", lista.nodos[i]->key);
			printf("%s\n", lista.nodos[i]->time);
		}
		i++;
	}
	
}

void mostrarTodo(tList lista) {
	
	mostrarListaMalloc(lista);
	mostrarListaMMap(lista);
	mostrarListaShared(lista);
}

void borrarLista(tList* lista) {
	
	if (!esListaVacia(*lista)) {
		while (lista->nodos[0] != NULL) {
			borrarNodo(0, lista);
		}
	}
}



