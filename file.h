// ---
// file.h
//
// Contains the class definition for
// a file, which will contain all
// the relevant data needed on the
// file, and methods to allow the
// user to perform operations on it.
// ---
#ifndef FILE_H
#define FILE_H
#include "diskItem.h"
#include <string>
#include <sys/stat.h>

class File : public DiskItem
{
	public:
		File(const char*);
		~File();

		//File operation functions:
		bool paste(std::string);
		bool deletef();

		//Getters:
		std::string getPath();
		std::string getName();
		unsigned int getSize();
};

#endif
