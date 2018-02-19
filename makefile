all: x

x:	list.o OS.o
	gcc -o x OS.o list.o

OS.o list.o: OS.o list.o
	gcc -c OS.c list.c

clean:
	rm -f *.o *.s x
