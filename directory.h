// ---
// directory.h
//
// Contains the class definition for
// a directory, a collection of DiskItems.
// ---

#ifndef DIRECTORY_H
#define DIRECTORY_H
#include "diskItem.h"
#include <vector>

class Directory : public DiskItem
{
	private:
		//The files the directory contains:
		std::vector <DiskItem*> _files;

		//The number of dotfiles in the directory:
		unsigned int _dotfiles;

	public:
		//Default constructor, takes a filename:
		Directory(const char*);

		//Takes a pointer to a directory object, creates a copy:
		Directory(Directory*);

		//Destructor:
		~Directory();

		//Reads the contents of the directory:
		void read();

		//Calculates the size of the directory:
		void calcSize();

		//Directory operation functions:
		bool paste(std::string);
		bool deletef();

		//Cleans the path to remove trailing '../':
		void cleanPath();

		//Getters:
		std::string getName();
		std::vector <DiskItem*>& getFiles();
		unsigned int getDotfiles();
};

#endif
