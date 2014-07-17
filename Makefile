CC=g++
FLAGS=-Wall -c
LIBS=-lncurses
DESTDIR=/usr/local
BIN=trilobite
OBJ=trilobite.o diskItem.o file.o directory.o

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LIBS) $(OBJ) -o $(BIN)

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
	rm $(DESTDIR)/bin/$(BIN)
	rm $(DESTDIR)/share/man/man1/$(BIN).1

install:
	if [ ! -d $(DESTDIR)/bin ]; then mkdir $(DESTDIR)/bin; fi
	install -m 0755 $(BIN) $(DESTDIR)/bin/
	if [ ! -d $(DESTDIR)/share/man/man1 ]; then mkdir -p $(DESTDIR)/share/man/man1; fi
	install -m 0644 $(BIN).1 $(DESTDIR)/share/man/man1/

clean:
	rm *.o
	rm $(BIN)
