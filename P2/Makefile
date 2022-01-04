#Definir cal é o compilador que queremos usar
CC=gcc

#Definir opcions de compilacion (unha "almoadilla" comenta 1 liña).
CFLAGS2 = -c
CFLAGS= -g -O0 -Wall
#CFLAGS= -O9
EXE= shell

#compilando e linkando ...
all: compile 

compile:
	@echo "## COMPILANDO ...."
	$(CC) $(CFLAGS2) list.h list.c
	$(CC) $(CFLAGS) -o $(EXE) list.o shell.c
	
run: compile
	@echo "## EXECUTANDO ...."
	valgrind --tool=memcheck --leak-check=full ./$(EXE)


#limpar lixo
clean:
	rm -f $(EXE) *.o *.c.bak
	
#empaqueta todos os arquivos
tar:
	tar czvf valgrindIni.tar.gz list.h list.c shell.c Makefile 
