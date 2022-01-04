//Tomé Maseda Dorado login:tome.maseda             34283930S
//Julián Barcia Facal login:julian.bfacal          46294725A


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


#define MAXC 4096

typedef struct {
	void* addr;
	size_t size;
	char* type;
	key_t key;
	int df;
	char* nameFile;
	char time[50];
	int id;
	
} tNodo;

typedef struct {
	tNodo* nodos[MAXC];
	int fin;
}tList;


tNodo inicializarNodo(tNodo nodo);
tNodo crearNodoMalloc(char* trozos[], void* addr);
tNodo crearNodoMMap(char* file, void* addr, size_t size, int df);
tNodo crearNodoCreateShared(key_t key, size_t size, void* addr, int id);
int esListaVacia(tList lista);
void crearListaVacia(tList* lista);
void insertarNodo(tNodo* nodo, tList* lista);
void borrarNodo(int pos, tList* lista);
int getPosicionSize(size_t size, tList lista);
int getPosicionAddr(char* addr, tList lista);
int getPosicionFich(char* nameFile, tList lista);
int getPosicionKey(key_t key, tList lista);
void mostrarListaMalloc(tList lista);
void mostrarListaMMap(tList lista);
void mostrarListaShared(tList lista);
void mostrarTodo(tList lista);

