CC=g++
FLAGS=-Wall -c
LIBS=-lncurses
SDIR=./

trilobite: trilobite.o diskItem.o file.o directory.o
	$(CC) $(LIBS) trilobite.o diskItem.o file.o directory.o -o trilobite

trilobite.o: $(SDIR)trilobite.cpp
	$(CC) $(FLAGS) trilobite.cpp 

diskItem.o: $(SDIR)diskItem.h $(SDIR)diskItem.cpp
	$(CC) $(FLAGS) diskItem.cpp

file.o: $(SDIR)file.h $(SDIR)file.cpp 
	$(CC) $(FLAGS) file.cpp

directory.o: $(SDIR)directory.h $(SDIR)directory.cpp
	$(CC) $(FLAGS) directory.cpp

.PHONY: clean install
clean:
	rm *.o

install:
	cp trilobite /usr/local/bin/
