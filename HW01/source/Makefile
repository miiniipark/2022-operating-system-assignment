app.out: main.o LinkedList.o ObtainInput.o
	gcc -o app.out main.o LinkedList.o ObtainInput.o

main.o: LinkedList.h main.c
	gcc -c -o main.o main.c

LinkedList.o: ObtainInput.h LinkedList.h LinkedList.c
	gcc -c -o LinkedList.o LinkedList.c

ObtainInput.o: ObtainInput.o ObtainInput.c
	gcc -c -o ObtainInput.o ObtainInput.c