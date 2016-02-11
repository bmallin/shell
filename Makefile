all: myshell

myshell: main.o shell.o
	gcc main.o shell.o -o myshell

main.o: main.c
	gcc -c main.c

shell.o: shell.c
	gcc -c shell.c

clean:
	rm *.o myshell
