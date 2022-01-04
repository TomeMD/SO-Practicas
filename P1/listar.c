//Tomé Maseda Dorado login:tome.maseda
//Julián Barcia Facal login:julian.bfacal

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>


#define MAX 50

char TipoFichero (mode_t m);

char * ConvierteModo3 (mode_t m);

int printinfo(int longListing, char* trozos); 

int showLine(int longListing, int recursiveMode, int listHidden, char* trozos);


int main(int argc, char **argv) 
{
	char cwd[MAX];
	int i = 0, error = 0;
	bool longListing = false;
    bool listHidden = true;
    bool recursiveMode = false;
    int numOptions = 0;
    int numFiles; 
    int primerIndice;
    char path[MAX] = "";
    struct stat st = {0};
    
    

				 
	for(i=1; i<argc; i++){
		if(!strcmp(argv[i],"-l")){
			longListing = true;
			numOptions ++;
		}
		if(!strcmp(argv[i],"-v")){
			listHidden = false;
			numOptions ++;
		}
		if(!strcmp(argv[i],"-r")){
			recursiveMode = true;
			numOptions ++;
		}
	}
	printf("\n");
     
	numFiles = argc - 1 - numOptions;
	primerIndice = numOptions + 1;
				 
	if(numFiles==0){
				
		showLine(longListing, recursiveMode, listHidden, getcwd(cwd, sizeof(cwd)));
		}else{
					
			for(i=primerIndice; i < primerIndice + numFiles; i++){
				lstat(argv[i], &st);
				if (S_ISDIR(st.st_mode)) {
					strcat(path,getcwd(cwd, sizeof(cwd)));
					strcat(path,"/");
					strcat(path,argv[i]);
					printf("%s\n", argv[i]);
					printf("******\n");
					error = showLine(longListing, recursiveMode, listHidden, path);
					if (error == 0) {chdir("..");}
					strcpy(path,"\0");
				} else {printf("Error: %s is not a directory\n\n", argv[i]);}
			}
		}
				

}


char TipoFichero (mode_t m) {

	switch (m&S_IFMT) { /*and bit a bit con los bits de formato,0170000 */
		case S_IFSOCK: return 's'; /*socket */
		case S_IFLNK: return 'l'; /*symbolic link*/
		case S_IFREG: return '-'; /* fichero normal*/
		case S_IFBLK: return 'b'; /*block device*/
		case S_IFDIR: return 'd'; /*directorio */
		case S_IFCHR: return 'c'; /*char device*/
		case S_IFIFO: return 'p'; /*pipe*/
		default: return '?'; /*desconocido, no deberia aparecer*/
	}
}


char * ConvierteModo3 (mode_t m) {
	char * permisos;
	permisos=(char *) malloc (12);
	strcpy (permisos,"---------- ");
	permisos[0]=TipoFichero(m);
	
	if (m&S_IRUSR) permisos[1]='r'; /*propietario*/
	if (m&S_IWUSR) permisos[2]='w';
	if (m&S_IXUSR) permisos[3]='x';
	if (m&S_IRGRP) permisos[4]='r'; /*grupo*/
	if (m&S_IWGRP) permisos[5]='w';
	if (m&S_IXGRP) permisos[6]='x';
	if (m&S_IROTH) permisos[7]='r'; /*resto*/
	if (m&S_IWOTH) permisos[8]='w';
	if (m&S_IXOTH) permisos[9]='x';
	if (m&S_ISUID) permisos[3]='s'; /*setuid, setgid y stickybit*/
	if (m&S_ISGID) permisos[6]='s';
	if (m&S_ISVTX) permisos[9]='t';
	
	return (permisos);
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
				printf("%s ",ConvierteModo3(st.st_mode));
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
