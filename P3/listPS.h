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
#include <sys/time.h>
#include <sys/resource.h>

#define MAX 100

typedef struct tNodoPS* tPos;

typedef struct tNodoPS{
	pid_t pid;
	char comando[MAX];
	char time[MAX];
	int stat;
	char state[MAX];
	int returnValue;
	tPos sig;
} tNodoPS;

typedef tPos tListPS;

tPos inicializarNodoPS(tPos nodo); 
tPos crearNodoPS(pid_t pid, char* args[], int stat, pid_t pid2);
void insertarNodoPS(tPos nodo, tListPS* lista);
void borrarNodoPS(tPos nodo, tListPS* lista);
void borrarListaPS(tListPS* lista);
void listarPS(tListPS lista);
tPos buscarPID(pid_t pid, tListPS lista);
void imprimirNodo(tPos nodo, tListPS lista);
char* NombreSenal(int sen);
void borrarTerm(tListPS* lista);
void borrarSig(tListPS* lista);
