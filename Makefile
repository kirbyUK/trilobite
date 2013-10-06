CC=g++
FLAGS=-Wall -c
SDIR=./
ODIR=$(SDIR)obj/

file.o: $(SDIR)file.h $(SDIR)file.cpp
	$(CC) $(FLAGS) file.cpp
	mv file.o $(ODIR)

directory.o: $(SDIR)directory.h $(SDIR)directory.cpp file.o
	$(CC) $(FLAGS) directory.cpp
	mv directory.o $(ODIR)
