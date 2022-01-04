//Tomé Maseda Dorado login:tome.maseda             34283930S
//Julián Barcia Facal login:julian.bfacal          46294725A

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <ftw.h>
#include <grp.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <ctype.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <signal.h>
#include "hist.h"
#include "list.h"
#include "listPS.h"


#define MAX 100
#define MAXC 4096
#define nlista 30
#define LEERCOMPLETO ((ssize_t)-1)


void inicializarTrozos(char* trozos[]);

void leerComando(char comando[], tHist* hist);

int escogerComando(char comando[]);

int TrocearCadena(char * cadena, char * trozos[]);

int BorrarRecursivo(char * trozos);

int printinfo(int longListing, char* trozos); 

int showLine(int longListing, int recursiveMode, int listHidden, char* trozos);

void asignarMem(char * trozos[], tList* lista);

void desasignarMem(char *trozos[], tList* lista);

void asignarMalloc(char * trozos[], tList* lista);

void desasignarAddr(char * trozos[], tList* lista);

void desasignarShared(char * trozos[], tList* lista);

void desasignarMMap(char * trozos[], tList* lista);

void desasignarMalloc(char * trozos[], tList* lista);

void Cmd_borrakey (char *args[]);

void autores(char* trozos[]);

void exec(char* trozos[]);

void exec2(char* trozos[]);

void procesarEntrada(int NumComando, char * trozos[], tHist* hist, tList* lista, tListPS* listPS);

void * MmapFichero (char * fichero, int protection, tList* lista);

void Cmd_AsignarCreateShared (char *arg[],tList* lista);

void Cmd_AsignarMmap (char *arg[],tList* lista);

void * ObtenerMemoriaShmget (key_t clave, size_t tam , tList* lista);

void liberarMemoria (tList* lista, tHist* hist, tListPS* listPS);

int main()
{	
	char comando[MAX]; strcpy(comando, "");
	char * trozos[MAX]; inicializarTrozos(trozos); 	
	tHist* pHist = malloc (sizeof(tHist)); crearHistVacio(pHist);
	tList* pLista = malloc (sizeof(tList)); crearListaVacia(pLista);
	tListPS* listPS = malloc (sizeof(tListPS)); *listPS = NULL;
	
	while (1) {
		printf("--> ");
		leerComando(comando, pHist);
		TrocearCadena(comando, trozos);			
		procesarEntrada(escogerComando(comando), trozos, pHist, pLista, listPS);
		printf("\n");
	}
	return 0;	
}

void inicializarTrozos(char* trozos[]) {
	for (int i = 0; i < MAX; i++) {
		trozos[i] = NULL;
	}
}

void liberarMemoria (tList* lista, tHist* hist, tListPS* listPS) {
	borrarHist(hist);
	borrarLista(lista);
	borrarListaPS(listPS);
	free(hist);
	free(lista);
	free(listPS);
}
void leerComando(char comando[], tHist* hist) {
	if (fgets(comando, MAX, stdin) == NULL) {
		perror("fgets");
	}
	insertarComando(comando, hist);	
}

int escogerComando(char comando[]) {
	char listaComandos[nlista][MAX] = {"autores", "pid", "cdir", "fecha", "hora", "hist", "fin", "end", "exit", 
								"crear","borrar", "info","listar","asignar", "desasignar","borrarkey","mem","volcar","llenar",
								"recursiva","rfich","wfich","priority","fork", "exec","pplano","splano", "listarprocs", "proc", 
								"borrarprocs"};
								
	for (int i = 0; i < nlista; i++) {
		if (strcmp(comando, listaComandos[i]) == 0) 
			return i;
	}
	return -1;	
}

void doRecursiva (int n){
	char automatico[2048];
	static char estatico[2048];
	printf ("parametro n:%d en %p\n",n,&n);
	printf ("array estatico en:%p \n",estatico);
	printf ("array automatico en %p\n",automatico);
	n--;
	if (n>0) doRecursiva(n);
}

ssize_t LeerFichero (char *fich, void *p, ssize_t n)  /*n=-1 indica que se lea todo*/{
	ssize_t  nleidos,tam=n;
	int df, aux;
	struct stat s;
	
	if (stat (fich,&s)==-1 || (df=open(fich,O_RDONLY))==-1){
		printf("Imposible leer fichero: Bad address");
		return ((ssize_t)-1);}
		
	if (n==LEERCOMPLETO)
		tam=(ssize_t) s.st_size;
		
	if ((nleidos=read(df,p, tam))==-1){
		aux=errno;
		close(df);
		errno=aux;
		printf("Imposible leer fichero: No such file or directory");
		return ((ssize_t)-1);
	}
	close (df);
	return (nleidos);
}
	
void Cmd_borrakey (char *args[]){
	key_t clave;
	int id;
	char *key=args[1];
		
	if (key==NULL || (clave=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE){
		printf ("   rmkey  clave_valida\n");
		return;
	}
	if ((id=shmget(clave,0,0666))==-1){
		perror ("shmget: imposible obtener memoria compartida");
		return;
	}
	if (shmctl(id,IPC_RMID,NULL)==-1)
		perror ("shmctl: imposible eliminar memoria compartida\n");
}


void * ObtenerMemoriaShmget (key_t clave, size_t tam, tList* lista){
	void * p;
	int aux,id,flags=0777;
	struct shmid_ds s;
	
	
	if (tam) /*si tam no es 0 la crea en modo exclusivo */
	flags=flags | IPC_CREAT | IPC_EXCL;
			/*si tam es 0 intenta acceder a una ya creada*/
	if (clave==IPC_PRIVATE)   /*no nos vale*/
	{errno=EINVAL; return NULL;}
	if ((id=shmget(clave, tam, flags))==-1) {return (NULL);}
	if ((p=shmat(id,NULL,0))==(void*) -1){
		aux=errno;   /*si se ha creado y no se puede mapear*/
		if (tam)     /*se borra */
		shmctl(id,IPC_RMID,NULL);
		errno=aux;
		return (NULL);
		}
		shmctl (id,IPC_STAT,&s);
		tNodo nodo;
		nodo = crearNodoCreateShared(clave, s.shm_segsz, p, id);
		insertarNodo(&nodo, lista);
	return (p);
}
	
void Cmd_AsignarCreateShared (char *arg[],tList* lista){
	key_t k;
	size_t tam=0;
	void *p;
	
	if (arg[2]==NULL || arg[3]==NULL){
		mostrarListaShared(*lista); return;}
		
		k=(key_t) atoi(arg[2]);
		
		if (arg[2]!=NULL)
		tam=(size_t) atoll(arg[3]);
		
		if ((p=ObtenerMemoriaShmget(k,tam,lista))==NULL)
			perror ("\nImposible obtener memoria shmget");
		else
			printf ("\nMemoria de shmget de clave %d asignada en %p\n",k,p);
	}
	
void * MmapFichero (char * fichero, int protection, tList* lista){
	
	int df, map=MAP_PRIVATE,modo=O_RDONLY;
	struct stat s;
	void *p;
	if (protection&PROT_WRITE)  modo=O_RDWR;
	if (stat(fichero,&s)==-1 || (df=open(fichero, modo))==-1) return NULL;
	if ((p=mmap (NULL,s.st_size, protection,map,df,0))==MAP_FAILED)
		{perror("mmap");return NULL;}
		tNodo nodo;
		nodo = crearNodoMMap(fichero, p, s.st_size, df);
		insertarNodo(&nodo, lista);
		return p;
	}

void Cmd_AsignarMmap (char *arg[],tList* lista){
	
	char *perm;
	void *p;
	int protection=0;
	if (arg[2]==NULL){
		mostrarListaMMap(*lista); 
		return;}
	if ((perm=arg[3])!=NULL && strlen(perm)<4) {
		if (strchr(perm,'r')!=NULL) protection|=PROT_READ;
		if (strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
		if (strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
		}
		
	if ((p=MmapFichero(arg[2],protection,lista))==NULL)
		perror ("\nImposible mapear fichero");
	else
		printf ("\nfichero %s mapeado en %p\n", arg[2], p);
		
}

int TrocearCadena(char * comando, char * trozos[]) { 	
	int i=1;
	if ((trozos[0]=strtok(comando," \n\t"))==NULL)
	return 0;
	while ((trozos[i]=strtok(NULL," \n\t"))!=NULL)
	i++;
	return i;
}

int BorrarRecursivo(char * trozos) {
	
		struct stat st;
		struct dirent *dir;
		DIR *d;
		
		stat(trozos,&st);
		int n = 0;
		
		if (stat(trozos,&st) == -1){
		perror("stat");
		return -1;}
		n = S_ISDIR(st.st_mode);
		if (n == 1) {
			if (chdir(trozos) == -1 ){
			perror("Chdir");
			return -1;}
			d = opendir(".");
			while ((dir = readdir(d)) != NULL) {
				if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
						BorrarRecursivo(dir->d_name);
						continue;
					} 	
			} 
			if (closedir(d) == -1){
				perror("closedir");
				return -1;
			}
			chdir("..");
			if(remove(trozos) == 0) {
				printf("El directorio '%s' se ha borrado satisfactoriamente\n\n", trozos);
			} else { 
				printf("ERROR. No se ha podido eliminar el directorio\n");
				return -1;
			}
		} else if(remove(trozos) == 0) {
				printf("El archivo '%s' se ha borrado satisfactoriamente\n", trozos);
			} else { 
				printf("ERROR. No se ha podido eliminar el archivo\n");
				return -1;
		}	
return 0;			  
}


int printinfo(int longListing, char* trozos) {
	
	char *buf;
	struct group *grp;
    struct passwd *pwd;
    char t[ 100 ] = "";
	struct stat st = {0};
	
	


		if (lstat(trozos, &st) != 0) {
			printf("Cannot access '%s': No such file or directory\n", trozos);
		}else{
			if (longListing) {
				printf("%lu ",st.st_ino);
				if (S_ISLNK(st.st_mode)){
					printf("l");
				}else if (S_ISDIR(st.st_mode)){ 
					printf("d");
				}else {printf("-");}
				printf( (st.st_mode & S_IRUSR) ? "r" : "-");
				printf( (st.st_mode & S_IWUSR) ? "w" : "-");
				printf( (st.st_mode & S_IXUSR) ? "x" : "-");
				printf( (st.st_mode & S_IRGRP) ? "r" : "-");
				printf( (st.st_mode & S_IWGRP) ? "w" : "-");
				printf( (st.st_mode & S_IXGRP) ? "x" : "-");
				printf( (st.st_mode & S_IROTH) ? "r" : "-");
				printf( (st.st_mode & S_IWOTH) ? "w" : "-");
				printf( (st.st_mode & S_IXOTH) ? "x " : "- ");
				printf("%lu ",st.st_nlink);
											
				if (getpwuid(st.st_uid) == NULL){
					perror("getpwuid");
					return -1;
				}else {pwd = getpwuid(st.st_uid) ;
				printf("%s ", pwd->pw_name);}
														
				if (getgrgid(st.st_gid)== NULL){
					perror("getgrgid");
					return -1;
				}else {
					grp = getgrgid(st.st_gid);
					printf("%s ", grp->gr_name);
				}
														
				printf("%lu ",st.st_size);
				strftime(t, 100, "%b %d %H:%M", localtime( &st.st_mtime));
				printf("%s ",t);
				if (S_ISLNK(st.st_mode)) {
					buf=malloc(2*_PC_PATH_MAX);
					readlink(trozos,buf,2*_PC_PATH_MAX);
					printf("%s -> %s\n" , trozos,buf);
					free(buf);
				}else printf("%s\n",trozos);
			
			} else {
				printf("%s ",trozos);
				printf("%lu \n",st.st_size);
			}
		}
	
return 0;
}

int showLine(int longListing, int recursiveMode, int listHidden, char* trozos){
	
	struct stat buf;
	struct stat direct;
    DIR *d;
    struct dirent *dir;
    char path[MAX] = "";

        
    if((lstat(trozos,&buf)) != 0){
        printf("Cannot access '%s': No such file or directory\n\n", trozos);
        return 1;
    } else {
		if (S_ISDIR(buf.st_mode)) {
			if (chdir(trozos) == -1) {
				perror("chdir");
				return -1;
			}
			
			if((d = opendir(trozos)) == NULL){
				perror("No se puede abrir el directorio");
				return 1;
			}
		
			while ((dir = readdir(d)) != NULL){
				if(dir->d_name[0] != '.' || listHidden) {
					printinfo(longListing, dir->d_name);
				}
			}
			if (closedir(d) == -1){
				perror("closedir");
				return -1;
			}
			
			printf("\n");
			
			if (recursiveMode) {
				d = opendir(trozos);
				while ((dir = readdir(d)) != NULL) {
					lstat(dir->d_name, &direct);
					if ((dir->d_name[0] != '.') && (S_ISDIR(direct.st_mode))) {
						printf("%s\n", dir->d_name);
						printf("******\n");
						strcat(path,trozos);
						strcat(path,"/");
						strcat(path,dir->d_name);
						showLine(longListing, recursiveMode, listHidden, path);
						chdir("..");
						strcpy(path,"\0");
					}
				}
			}
		} else {
			printinfo(longListing, trozos);
		}
		return 0;
	}


}

void asignarMalloc(char * trozos[], tList* lista) {
	

	if (trozos[2] == NULL) {
		mostrarListaMalloc(*lista);
	} else {
		void* address = (void*) malloc (atoi(trozos[2]));
		tNodo nodo;
		nodo = inicializarNodo(nodo);
		nodo = crearNodoMalloc(trozos, address);
		insertarNodo(&nodo, lista);
		printf("\nallocated %s at %p\n", trozos[2], address);
	}
}

void desasignarAddr(char * trozos[], tList* lista) {
	
	if (trozos[1] == NULL) {
		mostrarTodo(*lista);
	} else {
		int i = getPosicionAddr(trozos[1], *lista);
		if (i != -1) {
			int error;
			if (strcmp(lista->nodos[i]->type, "malloc") == 0) {
				free(lista->nodos[i]->addr);
				printf("\ndeallocated %ld at %p\n", lista->nodos[i]->size, trozos[1]);	
			} else if (strcmp(lista->nodos[i]->type, "mmap") == 0) {
				error = munmap(lista->nodos[i]->addr, lista->nodos[i]->size);
				if (error == -1) perror("munmap");
				error = close(lista->nodos[i]->df);
				if (error == -1) perror("close");
				printf("\nunmapped file %s at %s\n", lista->nodos[i]->nameFile, trozos[1]);
			} else if (strcmp(lista->nodos[i]->type, "shared memory") == 0) {
				shmctl(lista->nodos[i]->id, IPC_RMID, NULL);
				printf("\ndeallocated block (key %d) at %s\n", lista->nodos[i]->key, trozos[1]);
			}
			borrarNodo(i, lista);
		} else {mostrarTodo(*lista);}
	}
}

void desasignarShared(char * trozos[], tList* lista) {
	
	if (trozos[2] == NULL) {
		mostrarListaShared(*lista);
	} else {
		int i = getPosicionKey(atoi(trozos[2]), *lista);
		if (i != -1) {
			shmctl(lista->nodos[i]->id, IPC_RMID, NULL);
			printf("deallocated block (key %s) at %p\n", trozos[2], lista->nodos[i]->addr);
			borrarNodo(i, lista);
		} else {mostrarListaShared(*lista);}
	}
}

void desasignarMMap(char * trozos[], tList* lista) {
	
	if (trozos[2] == NULL) {
		mostrarListaMMap(*lista);
	} else {
		int i = getPosicionFich(trozos[2], *lista);
		if (i != -1) {
			int error = munmap(lista->nodos[i]->addr, lista->nodos[i]->size);
			if (error == -1) perror("munmap");
			error = close(lista->nodos[i]->df);
			if (error == -1) perror("close");
			printf("unmapped file %s at %p\n", trozos[2], lista->nodos[i]->addr);
			borrarNodo(i, lista);
		} else {mostrarListaMMap(*lista);}
	}
}

void desasignarMalloc(char * trozos[], tList* lista) {
	
	if (trozos[2] == NULL) {
		mostrarListaMalloc(*lista);
	} else {
		int i = getPosicionSize(atoi(trozos[2]), *lista);
		if (i != -1) {
			printf("deallocated %s at %p\n", trozos[2], lista->nodos[i]->addr);
			borrarNodo(i, lista);
		} else {mostrarListaMalloc(*lista);}
	}
}

void asignarMem(char *trozos[], tList* lista) {
	
	if (strcmp(trozos[1], "-malloc") == 0) {
		asignarMalloc(trozos, lista);
	} else if (strcmp(trozos[1], "-mmap") == 0) {
		Cmd_AsignarMmap(trozos,lista);
			
	} else if (strcmp(trozos[1], "-shared") == 0) {
		trozos[3] = "0";
		Cmd_AsignarCreateShared(trozos,lista);
			
	}else if (strcmp(trozos[1], "-createshared") == 0) {
		Cmd_AsignarCreateShared(trozos,lista);
			
	}
	
}

void desasignarMem(char *trozos[], tList* lista) {
	
	if (strcmp(trozos[1], "-malloc") == 0) {
		desasignarMalloc(trozos, lista);
	} else if (strcmp(trozos[1], "-mmap") == 0) {
		desasignarMMap(trozos, lista);
			
	} else if (strcmp(trozos[1], "-shared") == 0) {
		desasignarShared(trozos, lista);
			
	}else {
		desasignarAddr(trozos, lista);
	}
	
}

void autores(char* trozos[]) {
	if (trozos[1] == NULL) {
		printf("Tome Maseda Dorado:tome.maseda\n");
		printf("Julian Barcia Facal:julian.bfacal\n");
	} else if (strcmp(trozos[1], "-n") == 0) {
		printf("Tome Maseda Dorado\n");
		printf("Julian Barcia Facal\n");
	} else if (strcmp(trozos[1], "-l") == 0)   {
		printf("tome.maseda\n");
		printf("julian.bfacal\n");
	}	
}

void obtenerPID(char* trozos[]) {
	
	if (trozos[1] == NULL) { 
		if (getpid()== -1){
			perror("getpid"); 
		}						
		printf("Proceso en ejecucion: PID %d \n", getpid());
	} else {
		if (getppid() == -1) {
			perror("getppid");
		}
		printf("Proceso padre: PPID %d \n", getppid());	
	}
}

void obtenerCWD(char* trozos[]) {
	char cwd[MAX];
	if (trozos[1] == NULL){
		if (getcwd(cwd,sizeof(cwd)) == NULL){
			perror("getcwd");
		} else 
			printf("El directorio de trabajo actual es: %s \n",getcwd(cwd, sizeof(cwd)));
	} else {
		int n = chdir(trozos[1]);
		if (n == -1){perror("");}
	}
}

void fecha_hora(int fecha) {
	time_t tiempo = time(NULL);
	struct tm *tlocal = localtime(&tiempo);
	char output[11];
	if (fecha)
		strftime(output,11,"%d/%m/%y",tlocal);
	else 
		strftime(output,11,"%H:%M:%S",tlocal);
	printf("%s\n",output);	
}

void historico(char* trozos[], tHist* hist) {

	if (trozos[1] == NULL) {
		mostrarHist(*hist);
	} else if (strcmp(trozos[1], "-c") == 0) {
		borrarHist(hist);
	}	
}

int crear(char* trozos[]) {
	DIR *d;
	struct dirent *dir;
	d = opendir(".");
	int n;
	struct stat st = {0};
	FILE *fichero;
	
	if ((trozos[1] == NULL) || ((strcmp(trozos[1], "-d")  == 0)&& (trozos[2] == NULL))){ 
		while ((dir = readdir(d)) != NULL){
			printf("%s\n", dir->d_name);
		}
		if (closedir(d) == -1) {perror("closedir"); return -1;}
		
	}else if (strcmp(trozos[1], "-d")  == 0){
		if (stat(trozos[2], &st) == -1) {
			n = mkdir(trozos[2], 0700);
			if (n < 0 ) perror("Mkdir");
		} else printf(" Error : El directorio ya existe\n");						
	} else {
		fichero =fopen(trozos[1],"w");
		if (!fichero) {printf("Error : El archivo ya existe"); return -1;}
	}
	return 0;	
}

int existe(char* file) {
	DIR *d;
	struct dirent *dir;
	d = opendir(".");
	
	while ((dir = readdir(d)) != NULL){
		if (strcmp(dir->d_name, file) == 0) return 1;
	}
	if (closedir(d) == -1) perror("closedir");
	return 0;
}

void escribirFichero(char* trozos[], tList* lista) {
	char * creacion[3];
	if (strcmp(trozos[1],"-o") == 0) {
		if((trozos[2] == NULL) | (trozos[3] == NULL) | (trozos[4] == NULL)) 
			printf("Faltan parametros\n");
		else {
			creacion[0] = "";
			creacion[1] = trozos[2];
			if (crear(creacion) != -1) {
				int i = getPosicionAddr(trozos[3], *lista);
				if ((i != -1) && (strcmp(lista->nodos[i]->type, "mmap") == 0)) {
					int df;
					char addr[MAX];
					strcpy(addr, trozos[3]);
					ssize_t n = LeerFichero(lista->nodos[i]->nameFile, trozos[3], atoi(trozos[4]));
					if (n != -1) {
						if ((df=open(trozos[2],O_WRONLY)) != -1) {
							if ((n = write(df, trozos[3], n)) != -1)
								printf("Escritos %ld bytes en %s de %s\n", n, trozos[2], addr);
							else perror("write");
						} else perror("open");
					}
				} else printf("Error al leer: Bad memory address\n");
			}	
		}
	} else {
		if((trozos[2] == NULL) | (trozos[3] == NULL))
			printf("Faltan parametros\n");
		else {
			if (existe(trozos[1])) printf("Error: el archivo ya existe\n");
			else {
				creacion[0] = "";
				creacion[1] = trozos[1];
				if (crear(creacion) != -1) {
					int i = getPosicionAddr(trozos[2], *lista);
					if ((i != -1) && (strcmp(lista->nodos[i]->type, "mmap") == 0)) {
						int df;
						char addr[MAX];
						strcpy(addr, trozos[2]);
						ssize_t n = LeerFichero(lista->nodos[i]->nameFile, trozos[2], atoi(trozos[3]));
						if (n != -1) {
							if ((df=open(trozos[1],O_WRONLY)) != -1) {
								if ((n = write(df, trozos[2], n)) != -1)
									printf("Escritos %ld bytes en %s de %s\n", n, trozos[1], addr);
								else perror("write");
							} else perror("open");
						}
					} else printf("Error al leer: Bad memory address\n");
				}
			}
		}
	}
}

void mostrar(char* trozos[], tList* lista) {
	if(strcmp(trozos[1], "-malloc")  == 0) {
		mostrarListaMalloc(*lista);
	} else if(strcmp(trozos[1], "-shared")  == 0) {
		mostrarListaShared(*lista);
	} else if(strcmp(trozos[1], "-mmap")  == 0){
		mostrarListaMMap(*lista);
	} else if(strcmp(trozos[1], "-all")  == 0) {
		mostrarTodo(*lista);
	}
}

void volcar (char *arg[]){
	char *address;
	int size;
	
		address = (char*) strtoul(arg[1],NULL,16);

    if (arg[2] == NULL){
		   size = 25;
		} else size = atoi(arg[2]);
  
	int c = size / 25;
	int m = size % 25;
	int desp = 0;
	int cont = 0;
	if (size < 25) 
		desp = size;
	else desp = 25;
	
	for (int i = 0; i < size ; i= i + 25) {
		for(int j = 0; j< desp; j++){
			if(isprint(address[j+i])){
				 printf("%3c",(address[j+i]));
			}else {printf("   ");}
		}
		
		putchar('\n');
		for( int j = 0; j < desp; j++){
			if(isprint(address[j+i])){
				printf("%3X",(address[j+i]));
			} else printf("  0");	
			}
			putchar('\n');
		++cont;
		if (cont >= c) desp = m;
	}
		putchar('\n');
}

void llenar (char *arg[]){
	char *address;
	int valor;
	char * arg3;
	
	arg3 = (char *) arg[3];
	address = (char*) strtoul(arg[1],NULL,16);
	
	if (arg[2] == NULL) {
		memset(address,'A',128);
	} else if ((arg[2] != NULL) && (arg[3] == NULL) ) {
		memset(address,atoi(arg[2]),128);
	} else if ((arg[2] != NULL) && (arg[3] != NULL) ) {
			if (strncmp(arg[3], "0x" ,strlen("0x") ) == 0 ){
				valor =  strtoul(arg[3],NULL,16);
			} else  if (strlen(arg3) == 1) {
					for(int i = 0; i < strlen(arg3); i++){
					valor = (int) arg3[i];
					}
				} else valor = atoi(arg[3]);
				
		memset(address,valor,atoi(arg[2]));
			}    
}

void borrar(char * trozos[]) {
	DIR *d;
	struct dirent *dir;
	d = opendir(".");
	
	if ((trozos[1] == NULL) || ((strcmp(trozos[1], "-r")  == 0)&& (trozos[2] == NULL))){ 
		while ((dir = readdir(d)) != NULL){
			printf("%s\n", dir->d_name);
		}
		if (closedir(d) == -1){
			perror("closedir");
		}
					
	} else if(strcmp(trozos[1], "-r")  == 0) {	
		BorrarRecursivo(trozos[2]);
	} else {
					 
		if(remove(trozos[1]) == 0) {
			printf("El archivo o directorio '%s' se ha borrado satisfactoriamente\n", trozos[1]);
		} else {
			printf("ERROR. No se ha podido eliminar el archivo o directorio\n");
		}	  
	}	
}

int listar(char* trozos[]) {
	char cwd[MAX];
	char path[MAX] = "";
	struct stat st = {0};
	int numOptions = 0, numFiles, primerIndice, i, n;
	bool longListing = false;
    bool listHidden = true;
    bool recursiveMode = false;
    
	while (trozos[n] != NULL) {n++;}
				 
	for(i=1; i<n; i++){
		if(!strcmp(trozos[i],"-l")){
			longListing = true;
			numOptions ++;
		}
		if(!strcmp(trozos[i],"-v")){
			listHidden = false;
			numOptions ++;
		}
		if(!strcmp(trozos[i],"-r")){
			recursiveMode = true;
			numOptions ++;
		}
	}
	printf("\n");
     
	numFiles = n - 1 - numOptions;
	primerIndice = numOptions + 1;
				 
	if(numFiles==0){	
		showLine(longListing, recursiveMode, listHidden, getcwd(cwd, sizeof(cwd)));
	} else {
		for(i=primerIndice; i < primerIndice + numFiles; i++){
			if (lstat(trozos[i], &st) == -1){
				perror("lstat");
				return -1;
			}
			if (S_ISDIR(st.st_mode)) {
				if (getcwd(cwd,sizeof(cwd)) == NULL) {
					perror("getcwd");
					return -1;
				}
				strcat(path,getcwd(cwd, sizeof(cwd)));
				strcat(path,"/");
				strcat(path,trozos[i]);
				printf("%s\n", trozos[i]);
				printf("******\n");
				int error = showLine(longListing, recursiveMode, listHidden, path);
				if (error == 0) {chdir("..");}
					strcpy(path,"\0");
				} else {printf("Error: %s is not a directory\n\n", trozos[i]);}
		}
	}
	return 0;	
}

void pplano(char* trozos[], tList* pLista, tHist* pHist, tListPS* listPS) {
	
	pid_t pid;	
	
	if ((pid=fork()) == -1) 
		perror("fork");
	else if (pid == 0) {
		exec(trozos);
		liberarMemoria(pLista, pHist, listPS);
		exit(0);
	} else {
		if (waitpid(pid,NULL,0) == -1) perror("Waitpid");
	}		
}

void pplano2(char* trozos[], tList* pLista, tHist* pHist, tListPS* listPS) {
	
	pid_t pid;	
	
	if ((pid=fork()) == -1) 
		perror("fork");
	else if (pid == 0) {
		exec2(trozos);
		liberarMemoria(pLista, pHist, listPS);
		exit(0);
	} else {
		if (waitpid(pid,NULL,0) == -1) perror("Waitpid");
	}		
}

void splano(char * trozos[], tListPS* listPS, tList* pLista, tHist* pHist) {
	int stat;;
	pid_t pid;
		
	if ((pid = fork()) == -1) 
		perror("fork");
	else if (pid == 0){
		exec(trozos);
		liberarMemoria(pLista, pHist, listPS);
		exit(0);
	} else {
		pid_t pid2 = waitpid(pid, &stat, WNOHANG | WUNTRACED | WCONTINUED);
		if (pid2 != -1) {
			tPos nodoPS = crearNodoPS(pid, trozos + 1, stat, pid2);
			insertarNodoPS(nodoPS, listPS);
		} else perror("waitpid");
	}
}

void splano2(char * trozos[], tList* pLista, tHist* pHist, tListPS* listPS) {
	int stat;
	pid_t pid;
		
	if ((pid = fork()) == -1) 
		perror("fork");
	else if (pid == 0){
		exec2(trozos);
		liberarMemoria(pLista, pHist, listPS);
		exit(0);
	} else {
		pid_t pid2 = waitpid(pid, &stat, WNOHANG | WUNTRACED | WCONTINUED);
		if (pid2 != -1) {
			tPos nodoPS = crearNodoPS(pid, trozos, stat, pid2);
			insertarNodoPS(nodoPS, listPS);
		} else perror("waitpid");
	}	
}


void priority(char* trozos[]) {
	int n;
	
	if (trozos[2] != NULL) {
		if ((atoi(trozos[1]) != getpid()) && ((atoi(trozos[1]) != getppid()))) {
			n = setpriority(PRIO_PROCESS,atoi(trozos[1]),-1);
		} else n = setpriority(PRIO_PROCESS,atoi(trozos[1]),atoi(trozos[2]));
			if (n == 0)
				printf("Prioridad del proceso %d es %d\n", atoi(trozos[1]), getpriority(PRIO_PROCESS,atoi(trozos[1])));
			else perror("setpriority");
	} else printf("Prioridad del proceso %d es %d\n",atoi(trozos[1]), getpriority(PRIO_PROCESS,atoi(trozos[1])));	
} 

void exec(char* trozos[]) {
	
	if (*trozos[1] == '@') {
		if (setpriority(PRIO_PROCESS, 0, atoi(trozos[1] + 1)) == 0) 
			printf("Prioridad del proceso %d es %d\n", getpid(), getpriority(PRIO_PROCESS, 0));
		else perror("setpriority");
		
		if (execvp(trozos[2], trozos + 2) == -1) perror("execvp");
	} else {
		if (execvp(trozos[1], trozos + 1) == -1) perror("execvp");
	}	
}

void exec2(char* trozos[]) {
	if (trozos[0] != NULL) {
		if (*trozos[0] == '@') {
			if (setpriority(PRIO_PROCESS, getpid(), atoi(trozos[0] + 1)) == 0) 
				printf("Prioridad del proceso %d es %d\n", getpid(), getpriority(PRIO_PROCESS, getpid()));
			else perror("setpriority");
			if (execvp(trozos[1], trozos + 1) == -1) perror("execvp");
		} else if (execvp(trozos[0], trozos) == -1) perror("execvp");
	}	
}

void gestionarOtros(char* trozos[], tListPS* listPS, tList* pLista, tHist* pHist) {
	int i = 0, background = false;
	
	while (trozos[i] != NULL) {
		if (strcmp(trozos[i], "&") == 0) { 
			background = true;
			trozos[i] = NULL;
		} else i++;
	}
	
	if (background)	
		splano2(trozos, pLista, pHist, listPS);
	else 
		pplano2(trozos, pLista, pHist, listPS);
}

void proc(char* trozos[], tListPS* listPS) {
	int stat;
	
	if (trozos[1] == NULL)
		listarPS(*listPS);
	else if (strcmp(trozos[1], "-fg") != 0) {
		tPos nodo = buscarPID(atoi(trozos[1]), *listPS);
		if (nodo != NULL) imprimirNodo(nodo, *listPS);
		else printf("Error: PID doesn't exist\n");
	} else if (trozos[2] == NULL) {
		listarPS(*listPS);
	} else {
		tPos nodo = buscarPID(atoi(trozos[2]), *listPS);
		if (nodo != NULL) {
			if ((strcmp(nodo->state, "RUNNING") == 0)||(strcmp(nodo->state, "STOPPED") == 0)) {
				waitpid(atoi(trozos[2]), &stat, 0);
				if (WIFEXITED(stat))
					printf("Process %s TERMINATED NORMALLY. Return value = %d\n", trozos[2], WEXITSTATUS(stat));
				else if (WIFSIGNALED(stat))
					printf("Process %s TERMINATED BY SIGNAL %s\n", trozos[2], NombreSenal(WTERMSIG(stat)));
				borrarNodoPS(nodo, listPS);
			}
		} else printf("Error: PID doesn't exist\n");
	}
}

void borrarprocs(char* trozos[], tListPS* listPS) {
	if ((trozos[1] != NULL)&&(trozos[2] == NULL)) {
		if (strcmp(trozos[1], "-term") == 0)
			borrarTerm(listPS);
		else if (strcmp(trozos[1], "-sig") == 0)
			borrarSig(listPS);
		else printf("Comando no reconocido\n");
	}
}

void procesarEntrada(int NumComando, char * trozos[], tHist* hist, tList* lista, tListPS* listPS) {
	char cwd[MAX];
	int n, i;
	char addr[MAX];
	int status;
	pid_t pid;

	switch (NumComando)
	{
		 case 0: autores(trozos); break;
				 	
		 case 1: obtenerPID(trozos); break;
				 
		 case 2: obtenerCWD(trozos); break;
				 
		 case 3: fecha_hora(1); break;
				 
		 case 4: fecha_hora(0); break;
				 
		 case 5: historico(trozos, hist); break;
		 
		 case 6: liberarMemoria(lista, hist, listPS); exit(0);
				 
		 case 7: liberarMemoria(lista, hist, listPS); exit(0);
				 
		 case 8: liberarMemoria(lista, hist, listPS); exit(0);
				 
		 case 9: crear(trozos); break;
			 
		case 10: borrar(trozos); break;
		
		case 11: while (trozos[n] != NULL) {n++;}
					for (int i = 1; i < n; i++) {
						printinfo(1, trozos[i]);
					}
				 break;
				 
		case 12: listar(trozos); break;
		
		case 13: if (trozos[1] == NULL) {
					mostrarTodo(*lista)	;
				 } else {asignarMem(trozos, lista);}
				 break;
		
		case 14: if (trozos[1] == NULL) {
					 mostrarTodo(*lista);
				 } else {desasignarMem(trozos, lista);}
				 break;
		
		case 15:Cmd_borrakey(trozos);
				break;
					
		case 16: if (trozos[1] != NULL) {
					 mostrar(trozos, lista);
				 } else {
					 printf("Funciones programa:  ");
					 printf("%15p  ", &(autores)); printf("%15p  ", &(fecha_hora)); printf("%15p\n", &(obtenerPID));
					 printf("Variables globales:  ");
					 printf("%15p  ", &(*hist)); printf("%15p  ", &(*trozos)); printf("%15p\n", &(*lista));
					 printf("Variables locales:  ");
					 printf("%16p  ", &cwd); printf("%15p  ", &i); printf("%15p\n", &n); 
				 }
				 break;
			
		case 17: volcar(trozos); break;
			
		case 18: llenar(trozos); break;
		case 19: if (trozos[1]!=NULL) {
					doRecursiva(atoi(trozos[1]));}
					break;
					
		case 20 : strcpy(addr, trozos[2]);
				  if (trozos[3] != NULL) {
					  if ((n = LeerFichero(trozos[1], trozos[2], atoi(trozos[3]))) != -1) 
						  printf("leidos %d bytes de %s en %s\n", n, trozos[1], addr);
				  } else {
					  if ((n = LeerFichero(trozos[1], trozos[2], (ssize_t) -1)) != -1) 
						  printf("leidos %d bytes de %s en %s\n", n, trozos[1], addr);
				  }
				  break;
				  
		case 21 : if (trozos[1] != NULL)
					 escribirFichero(trozos, lista); 
				  else printf("Faltan parametros\n");
				  break; 
				  
		case 22: priority(trozos);
				 break;
		
		case 23: if ((pid = fork()) == -1) perror("fork");
				 else if (pid != 0) waitpid(pid, &status, 0);
				 break;
		
		case 24: exec(trozos); 
				 break;
		
		case 25: pplano(trozos, lista, hist, listPS);
				 break;
		
		case 26: splano(trozos, listPS, lista, hist);
				 break;
				 
		case 27: listarPS(*listPS); 
				 break;
		
		case 28: proc(trozos, listPS);
				 break;
				 
		case 29: borrarprocs(trozos, listPS);
				 break;
				 
		default: gestionarOtros(trozos, listPS, lista, hist); 
				 break;
	}
}


