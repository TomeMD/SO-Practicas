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
#include "hist.h"



tDato inicializarDato(tDato dato) {
	strcpy(dato.comando, "");
	return dato;
}

tDato crearDato(char comando[MAX]) {
	tDato dato;
	
	strcpy(dato.comando, comando);
	return dato;
}

int esHistVacio(tHist hist) {
	return (hist.fin == -1);
}

void crearHistVacio(tHist* hist) {
	
	for (int i = 0; i < MAXC; i++) hist->datos[i] = NULL;
	hist->fin = -1;
}

void insertarComando(char comando[MAX], tHist* hist) {
	if ((hist->fin + 1) > MAXC) {
		printf("Error: lista llena.\n");
	} else {
		hist->fin+=1;
		hist->datos[hist->fin] = malloc (sizeof(tDato));
		strcpy(hist->datos[hist->fin]->comando, comando);
	}
}

void borrarDato(int pos, tHist* hist) {
		
	if (hist->datos[pos] == NULL) {
		printf("Error: el dato no existe.");
	} else {
		free(hist->datos[pos]);
		for (int i = pos; i < hist->fin; i++) {
			hist->datos[i] = hist->datos[i+1];
		}
		hist->datos[hist->fin] = NULL;
		hist->fin--;
	}		
}

void borrarHist(tHist* hist) {
	
	if (!esHistVacio(*hist)) {
		while (hist->datos[0] != NULL) {
			borrarDato(0, hist);
		}
	}
}

void mostrarHist(tHist hist) {
	int i = 0;
	
	while ((i <= hist.fin) && (hist.datos[i] != NULL)) {
		printf("%s", hist.datos[i]->comando);
		i++;
	}	
}
