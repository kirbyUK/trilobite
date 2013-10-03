#ifndef DIRECTORY_H
#define DIRECTORY_H
#include "file.h"
#include <vector>

class Directory : public File
{
	private:
		//The files the directory contains:
		std::vector <File*> _files;

	public:
		//Constructor:
		Directory(const char*);

		//Destructor:
		~Directory();

		//Directory operation functions:
		bool paste(std::string);
		bool deletef();

		//Returns the list of files:
		std::vector <File*>& getFiles();
};

#endif
