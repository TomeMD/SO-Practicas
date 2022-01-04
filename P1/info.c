//Tomé Maseda Dorado login:tome.maseda
//Julián Barcia Facal login:julian.bfacal

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <ftw.h>
#include <grp.h>
#include <pwd.h>

int printinfo(int longListing, char* trozos);

int main(int argc, char **argv) 
{
	int i = 0;
	
	while (argv[i] != NULL) {i++;}
	for (int i = 1; i < argc; i++) {
		printinfo(1, argv[i]);
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
