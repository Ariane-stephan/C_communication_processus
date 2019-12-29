
all : executable
executable : main.o msg_file.o structure.o
	gcc -o executable main.o msg_file.o structure.o -lrt -lpthread
main.o : main.c msg_file.h structure.h
	gcc -c main.c -Wall -g
msg_file.o : msg_file.c msg_file.h structure.h
	gcc -c msg_file.c -Wall -g
structure.o : structure.c structure.h
	gcc -c structure.c -Wall -g
clean :
	rm main.o msg_file.o structure.o executable
