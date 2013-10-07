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

	public:
		//Constructor: 
		Directory(const char*);

		//Destructor:
		~Directory();

		//Directory operation functions:
		bool paste(std::string);
		bool deletef();

		//Getters:
		std::string getName();
		std::vector <DiskItem*>& getFiles();
};

#endif
