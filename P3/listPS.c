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
#include <sys/wait.h>
#include <limits.h>
#include "listPS.h"

struct SEN{
	char *nombre;
	int senal;
};

static struct SEN sigstrnum[]={
	{"HUP", SIGHUP},
	{"INT", SIGINT},
	{"QUIT", SIGQUIT},
	{"ILL", SIGILL},
	{"TRAP", SIGTRAP},
	{"ABRT", SIGABRT},
	{"IOT", SIGIOT},
	{"BUS", SIGBUS},
	{"FPE", SIGFPE},
	{"KILL", SIGKILL},
	{"USR1", SIGUSR1},
	{"SEGV", SIGSEGV},
	{"USR2", SIGUSR2},
	{"PIPE", SIGPIPE},
	{"ALRM", SIGALRM},
	{"TERM", SIGTERM},
	{"CHLD", SIGCHLD},
	{"CONT", SIGCONT},
	{"STOP", SIGSTOP},
	{"TSTP", SIGTSTP},
	{"TTIN", SIGTTIN},
	{"TTOU", SIGTTOU},
	{"URG", SIGURG},
	{"XCPU", SIGXCPU},
	{"XFSZ", SIGXFSZ},
	{"VTALRM", SIGVTALRM},
	{"PROF", SIGPROF},
	{"WINCH", SIGWINCH},
	{"IO", SIGIO},
	{"SYS", SIGSYS},
	/*senales que no hay en todas partes*/
	
	#ifdef SIGPOLL
		{"POLL", SIGPOLL},
	#endif
	#ifdef SIGPWR
		{"PWR", SIGPWR},
	#endif
	#ifdef SIGEMT
		{"EMT", SIGEMT},
	#endif
	#ifdef SIGINFO
		{"INFO", SIGINFO},
	#endif
	#ifdef SIGSTKFLT
		{"STKFLT", SIGSTKFLT},
	#endif
	#ifdef SIGCLD
		{"CLD", SIGCLD},
	#endif
	#ifdef SIGLOST
		{"LOST", SIGLOST},
	#endif
	#ifdef SIGCANCEL
		{"CANCEL", SIGCANCEL},
	#endif
	#ifdef SIGTHAW
		{"THAW", SIGTHAW},
	#endif
	#ifdef SIGFREEZE
		{"FREEZE", SIGFREEZE},
	#endif
	#ifdef SIGLWP
		{"LWP", SIGLWP},
	#endif
	#ifdef SIGWAITING
		{"WAITING", SIGWAITING},
	#endif
		{NULL,-1},	
}; /*fin array sigstrnum */

int eslistaPSVacia(tListPS lista) {
	return (lista == NULL);
}

tPos anterior(tPos nodo, tListPS lista) {
	tPos aux = lista;
	
	while (aux->sig != nodo)
		aux = aux->sig;
	return aux;
}

tPos inicializarNodoPS(tPos nodo) {
	nodo->pid = 0;
	strcpy(nodo->comando, "");
	strcpy(nodo->time, "");
	nodo->stat = 0;
	strcpy(nodo->state, "");
	nodo->returnValue = INT_MAX;
	nodo->sig = NULL;
	
	return nodo;
}

int actualizarStat(pid_t pid) {
	int stat;
	

	if (waitpid(pid, &stat, WNOHANG | WUNTRACED | WCONTINUED) == 0) 
		return INT_MAX;
	return stat;
	
}

char* evaluarEstado(int stat) {
	
	if (stat == INT_MAX)
		return "RUNNING";
	else if (WIFSTOPPED(stat)) 
		return "STOPPED";
	else if (WIFSIGNALED(stat)) 
		return "TERMINATED BY SIGNAL";
	else if (WIFEXITED(stat)) 
		return "TERMINATED NORMALLY";
	
	return "";
	
}

int actualizarRetorno(char state[], int stat) {
	
	if (strcmp(state, "TERMINATED NORMALLY") == 0)
		return WEXITSTATUS(stat);
	else if (strcmp(state, "TERMINATED BY SIGNAL") == 0)
		return WTERMSIG(stat);
	else if (strcmp(state, "STOPPED") == 0)
		return WSTOPSIG(stat);
		
	return stat;	
}

tPos crearNodoPS(pid_t pid, char* args[], int stat, pid_t pid2) {
	tPos nodo = malloc (sizeof(struct tNodoPS));
	int i = 0;
	time_t tiempo = time(NULL);
	struct tm* timer = localtime(&tiempo);
	
	nodo = inicializarNodoPS(nodo);
	if ((args[0] != NULL)&&(*args[0] == '@')) args += 1;
	nodo->pid = pid;
	while (args[i] != NULL) {
		strcat(nodo->comando, args[i]);
		strcat(nodo->comando, " ");
		i++;
	}
	strftime(nodo->time, 50,"%a %b %d %H:%M:%S %Y", timer);
	nodo->stat = stat;
	if (pid2 == 0) strcpy(nodo->state, "RUNNING");
	else strcpy(nodo->state, evaluarEstado(stat));
	nodo->returnValue = actualizarRetorno(nodo->state, stat);
	nodo->sig = NULL;
	
	return nodo;
}

void insertarNodoPS(tPos nodo, tListPS* lista) {
	tPos aux;
	
	if (eslistaPSVacia(*lista)) {
		*lista = nodo;
	} else {
		aux = *lista;
		while (aux->sig != NULL)
			aux = aux->sig;
		aux->sig = nodo;
	}
}

void borrarNodoPS(tPos nodo, tListPS* lista) {
	
	if (!eslistaPSVacia(*lista)) {
		if (*lista == nodo) {
			*lista = nodo->sig;
			free(nodo);
			nodo = NULL;
		} else {
			tPos ant = anterior(nodo, *lista);
			ant->sig = nodo->sig;
			free(nodo);
			nodo = NULL;
		}
	}
}

void borrarListaPS(tListPS* lista) {
	
	while (!eslistaPSVacia(*lista)) {
		borrarNodoPS(*lista, lista);
	}
}

char* NombreSenal(int sen) {/*devuelve el nombre de la señal a partir del nombre*/
	/*para sitios donde no hay sig2str*/
	int i;
	for (i=0; sigstrnum[i].nombre!=NULL; i++)
		if (sen==sigstrnum[i].senal)
			return sigstrnum[i].nombre;
	return ("SIGUNKNOWN");
}

void imprimirRetorno(char state[], int rValue) {
	
	if (strcmp(state, "TERMINATED NORMALLY") == 0)
		printf("%d\n", rValue);
	else if (strcmp(state, "TERMINATED BY SIGNAL") == 0)
		printf("%s\n", NombreSenal(rValue));
	else if (strcmp(state, "STOPPED") == 0)
		printf("%s\n", NombreSenal(rValue));	
}

void listarPS(tListPS lista) {
	tPos aux = lista;
	
	
	while (aux != NULL) {
		printf("%d ", aux->pid);
		printf("%d ", getpriority(PRIO_PROCESS, aux->pid));
		printf("%s ", aux->comando);
		printf("%s ", aux->time);
		if ((strcmp(aux->state, "RUNNING") == 0)||(strcmp(aux->state, "STOPPED") == 0)) {
			aux->stat = actualizarStat(aux->pid);
			strcpy(aux->state, evaluarEstado(aux->stat));
			aux->returnValue = actualizarRetorno(aux->state, aux->stat);
		}
		printf("%s ", aux->state);
		if (aux->returnValue != INT_MAX)
			imprimirRetorno(aux->state, aux->returnValue);
		else printf("\n");
		aux = aux->sig;
	}
}

tPos buscarPID(pid_t pid, tListPS lista) {
	tPos aux = lista;
	
	while (aux != NULL) {
		if (aux->pid == pid) return aux;
		aux = aux->sig;
	}
	return NULL;
}

void imprimirNodo(tPos nodo, tListPS lista) {
	
	printf("%d ", nodo->pid);
	printf("%d ", getpriority(PRIO_PROCESS, nodo->pid));
	printf("%s ", nodo->comando);
	printf("%s ", nodo->time);
	if ((strcmp(nodo->state, "RUNNING") == 0)||(strcmp(nodo->state, "STOPPED") == 0)) {
		nodo->stat = actualizarStat(nodo->pid);
		strcpy(nodo->state, evaluarEstado(nodo->stat));
		nodo->returnValue = actualizarRetorno(nodo->state, nodo->stat);
	}
	printf("%s ", nodo->state);
	if (nodo->returnValue != INT_MAX)
		imprimirRetorno(nodo->state, nodo->returnValue);
	else printf("\n");
		
}

void borrarTerm(tListPS* lista) {
	tPos aux = *lista, borrado = NULL;
	
	while (aux != NULL) {
		if (strcmp(aux->state, "TERMINATED NORMALLY") == 0) {
			borrado = aux;
			aux = aux->sig;
			borrarNodoPS(borrado, lista);
		} else aux = aux->sig;
	}
}

void borrarSig(tListPS* lista) {
	
	tPos aux = *lista, borrado = NULL;
	
	while (aux != NULL) {
		if (strcmp(aux->state, "TERMINATED BY SIGNAL") == 0) {
			borrado = aux;
			aux = aux->sig;
			borrarNodoPS(borrado, lista);
		} else aux = aux->sig;
	}
	
}
