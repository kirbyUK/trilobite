CC=g++
FLAGS=-Wall -c
LIBS=-lncurses
SDIR=./

trilobite: trilobite.o file.o directory.o diskItem.o
	$(CC) $(LIBS) file.o directory.o diskItem.o -o trilobite

trilobite.o: $(SDIR)trilobite.cpp
	$(CC) $(FLAGS) trilobite.cpp 

diskItem.o: $(SDIR)diskItem.h $(SDIR)diskItem.cpp
	$(CC) $(FLAGS) diskItem.cpp

file.o: $(SDIR)file.h $(SDIR)file.cpp 
	$(CC) $(FLAGS) file.cpp

directory.o: $(SDIR)directory.h $(SDIR)directory.cpp
	$(CC) $(FLAGS) directory.cpp

.PHONY: clean 
clean:
	rm *.o
