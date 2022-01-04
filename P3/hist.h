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
#define MAX 100

typedef struct {
	char comando[MAX];
} tDato;

typedef struct {
	tDato* datos[MAXC];
	int fin;
}tHist;

tDato inicializarDato(tDato dato);
tDato crearDato(char comando[MAX]);
int esHistVacio(tHist hist);
void crearHistVacio(tHist* hist);
void insertarComando(char comando[MAX], tHist* hist);
void borrarDato(int pos, tHist* hist);
void borrarHist(tHist* hist);
void mostrarHist(tHist hist);


