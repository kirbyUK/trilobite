CC=g++
FLAGS=-Wall -c
LIBS=-lncurses
DESTDIR=/usr/local

all: trilobite

trilobite: trilobite.o diskItem.o file.o directory.o
	$(CC) $(LIBS) trilobite.o diskItem.o file.o directory.o -o trilobite

trilobite.o: trilobite.cpp
	$(CC) $(FLAGS) trilobite.cpp 

diskItem.o: diskItem.h diskItem.cpp
	$(CC) $(FLAGS) diskItem.cpp

file.o: file.h file.cpp 
	$(CC) $(FLAGS) file.cpp

directory.o: directory.h directory.cpp
	$(CC) $(FLAGS) directory.cpp

deinstall: uninstall
uninstall:
	rm $(DESTDIR)/bin/trilobite

install:
	install -m 0755 trilobite $(DESTDIR)/bin/

clean:
	rm *.o
	rm trilobite
