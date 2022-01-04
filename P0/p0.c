//Tomé Maseda Dorado login:tome.maseda
//Julián Barcia Facal login:julian.bfacal

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX 50
#define MAXC 4096
#define nlista 9

int TrocearCadena(char * cadena, char * trozos[]);

int procesarEntrada(int NumComando, char * trozos[], int terminado, char * hist[]);


int main()
{
	char comando[MAX]; 
	char * trozos[MAX]; 
	char lista[nlista][MAX] = {"autores", "pid", "cdir", "fecha", "hora", "hist", "fin", "end", "exit"};
	int NumComando;
	int terminado = false; 
	char * hist[MAXC];
	int n;

	
	while (!terminado) {
		printf("--> ");
		fgets(comando, MAX, stdin);
		
		n = 0;
		while (hist[n] != NULL) {n++;}
		hist[n] = (char *) malloc (sizeof(char[MAXC]));
		for (int i = 0; i < strlen(comando); i++) {
			*hist[n] = comando[i];
			hist[n] = hist[n] + 1;
			
		}
		hist[n] = hist[n] - strlen(comando);
			
		
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

int procesarEntrada(int NumComando, char * trozos[], int terminado, char * hist[])
{
	time_t tiempo = time(NULL); 
	struct tm *tlocal = localtime(&tiempo); 
	char cwd[MAX];
	char output[11];
	int i = 0, j = 1;	

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
					chdir(trozos[1]);
					perror("");
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
						printf("%i: %s\n", j , hist[i]);
						i++;
						j++;
					}
				 } else if (strcmp(trozos[1], "-c") == 0) {
					while (hist[i] != NULL) {
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
				 
		 case 9: break;
						
	}
	
	return terminado;
}
