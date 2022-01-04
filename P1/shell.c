//Tomé Maseda Dorado login:tome.maseda
//Julián Barcia Facal login:julian.bfacal

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ftw.h>
#include <grp.h>
#include <pwd.h>



#define MAX 100
#define MAXC 4096
#define nlista 14


int TrocearCadena(char * cadena, char * trozos[]);

void BorrarRecursivo(char * trozos);

int printinfo(int longListing, char* trozos); 

int showLine(int longListing, int recursiveMode, int listHidden, char* trozos);

int procesarEntrada(int NumComando, char * trozos[], int terminado, char * hist[]);

int main()
{	
	char * comando; 
	char * trozos[MAX]; 
	char lista[nlista][MAX] = {"autores", "pid", "cdir", "fecha", "hora", "hist", "fin", "end", "exit", "crear","borrar", "info","listar"};
	int NumComando;
	int terminado = false; 
	char * hist[MAXC];
	int n;

	

	while (!terminado) {

		comando = (char *) malloc (MAX*sizeof(char));
		printf("--> ");
		fgets(comando, MAX, stdin);
		
		n = 0;
		while (hist[n] != NULL) {n++;}
		hist[n] = (char *) malloc ((strlen(comando) - 1)*sizeof(char));
		
		for(int i = 0; i < (strlen(comando) - 1); i++) {
			*(hist[n]+i) = *(comando+i);
		}
		*(hist[n]+strlen(comando)-1) = '\0';
	
			
		TrocearCadena(comando, trozos);		
		for (int i = 0; i < nlista; i++) 
		{
			
			NumComando = nlista;
			
			if (strcmp(comando, lista[i]) == 0) {
				NumComando = i;
				break;
			}
		}	
		printf("\n");
		
		terminado = procesarEntrada(NumComando, trozos, terminado, hist);
		printf("\n");
	}
	
	return 0;
}


int TrocearCadena(char * comando, char * trozos[]) { 	
	int i=1;
	if ((trozos[0]=strtok(comando," \n\t"))==NULL)
	return 0;
	while ((trozos[i]=strtok(NULL," \n\t"))!=NULL)
	i++;
	return i;
}

void BorrarRecursivo(char * trozos) {
	
		struct stat st;
		struct dirent *dir;
		DIR *d;
		stat(trozos,&st);
		int n = 0;
		
		stat(trozos,&st);
		n = S_ISDIR(st.st_mode);
		if (n == 1) {
			chdir(trozos);
			d = opendir(".");
			while ((dir = readdir(d)) != NULL) {
				if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
						BorrarRecursivo(dir->d_name);
						continue;
					} 	
			} 
			closedir(d);
			chdir("..");
			if(remove(trozos) == 0) {
				printf("El directorio '%s' se ha borrado satisfactoriamente\n\n", trozos);
			} else { 
				printf("ERROR. No se ha podido eliminar el directorio\n");
			}
		} else if(remove(trozos) == 0) {
				printf("El archivo '%s' se ha borrado satisfactoriamente\n", trozos);
			} else { 
				printf("ERROR. No se ha podido eliminar el archivo\n");
		}				  
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
			chdir(trozos);
			if((d = opendir(trozos)) == NULL){
				perror("No se puede abrir el directorio");
				return 1;
			}
		
			while ((dir = readdir(d)) != NULL){
				if(dir->d_name[0] != '.' || listHidden) {
					printinfo(longListing, dir->d_name);
				}
			}
			closedir(d);
			
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

int procesarEntrada(int NumComando, char * trozos[], int terminado, char * hist[])
{
	time_t tiempo = time(NULL); 
	struct tm *tlocal = localtime(&tiempo); 
	char cwd[MAX];
	char output[11];
	int i = 0, n = 0, error = 0;
	DIR *d;
	struct dirent *dir;
	struct stat st = {0};
    d = opendir(".");
    FILE *fichero;
    bool longListing = false;
    bool listHidden = true;
    bool recursiveMode = false;
    int numOptions = 0;
    int numFiles; 
    int primerIndice;
    char path[MAX] = "";
    
	switch (NumComando)
	{
		 case 0: if (trozos[1] == NULL) 
				{
				    printf("Tome Maseda Dorado:tome.maseda\n");
					printf("Julian Barcia Facal:julian.bfacal\n");
				} else {
					if (strcmp(trozos[1], "-n") == 0) {
						printf("Tome Maseda Dorado\n");
						printf("Julian Barcia Facal\n");
					} else if (strcmp(trozos[1], "-l") == 0)   {
						printf("tome.maseda\n");
						printf("julian.bfacal\n");
					}
				}
				break;
				
						
		 case 1: if (trozos[1] == NULL) {
					printf("Proceso en ejecucion: PID %d \n", getpid());
				 } else {
				    printf("Proceso padre: PPID %d \n", getppid());
				 }
		 
				
				 break;
				 
		 case 2: if (trozos[1] == NULL){
					printf("El directorio de trabajo actual es: %s \n",getcwd(cwd, sizeof(cwd)));
				 } else {
					n = chdir(trozos[1]);
					if (n == -1){perror("");}
				 }
				 break;
				 
		 case 3: strftime(output,11,"%d/%m/%y",tlocal);
				 printf("%s\n",output);
				 break;
				 
		 case 4: strftime(output,11,"%H:%M:%S",tlocal);
				 printf("%s\n",output);
				 break;
				 
		 case 5: if (trozos[1] == NULL) {
					while (hist[i] != NULL) {
						printf("%s\n", hist[i]);
						i++;
					}
				 } else if (strcmp(trozos[1], "-c") == 0) {
					while (hist[i] != NULL) {
						free(hist[i]);
						hist[i] = NULL;
						i++;
					}
				 }
				 break;
		 
		 case 6: terminado = true; 
				 break;
				 
		 case 7: terminado = true;
				 break;
				 
		 case 8: terminado = true;
				 break;
				 
		 case 9: if ((trozos[1] == NULL) || ((strcmp(trozos[1], "-d")  == 0)&& (trozos[2] == NULL))){ 
					 while ((dir = readdir(d)) != NULL){
						 printf("%s\n", dir->d_name);
					 }
					 closedir(d);
				 }else if (strcmp(trozos[1], "-d")  == 0){
					if (stat(trozos[2], &st) == -1) {
						n = mkdir(trozos[2], 0700);
						if (n < 0 ) {perror("Mkdir");}
					} else { 
					printf(" Error : El directorio ya existe\n");
				}
								
				} else {fichero =fopen(trozos[1],"w");
					if (!fichero) {printf("Error : El archivo ya existe");}
				}
				 break;
			 
		case 10: if ((trozos[1] == NULL) || ((strcmp(trozos[1], "-r")  == 0)&& (trozos[2] == NULL))){ 
					while ((dir = readdir(d)) != NULL){
						 printf("%s\n", dir->d_name);
					}
					closedir(d);
					
				 } else if(strcmp(trozos[1], "-r")  == 0) {	
						BorrarRecursivo(trozos[2]);
				 } else {
					 
					if(remove(trozos[1]) == 0) {
						printf("El archivo o directorio '%s' se ha borrado satisfactoriamente\n", trozos[1]);
					} else {
						printf("ERROR. No se ha podido eliminar el archivo o directorio\n");
					}	  
				 }
				 break;
		
		case 11: while (trozos[n] != NULL) {n++;}
					for (int i = 1; i < n; i++) {
						printinfo(1, trozos[i]);
					}
				 break;
				 
		case 12: while (trozos[n] != NULL) {n++;}
				 
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
				 }else{
					
					for(i=primerIndice; i < primerIndice + numFiles; i++){
						lstat(trozos[i], &st);
						if (S_ISDIR(st.st_mode)) {
							strcat(path,getcwd(cwd, sizeof(cwd)));
							strcat(path,"/");
							strcat(path,trozos[i]);
							printf("%s\n", trozos[i]);
							printf("******\n");
							error = showLine(longListing, recursiveMode, listHidden, path);
							if (error == 0) {chdir("..");}
							strcpy(path,"\0");
						} else {printf("Error: %s is not a directory\n\n", trozos[i]);}
					}
				 }
				
				break;
		
		case 13: break;
	}
	return terminado;
}

