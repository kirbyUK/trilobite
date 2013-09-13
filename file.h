// ---
// file.h
//
// Contains the class defintion for
// a file, which will contain all
// the relevant data needed on the
// file, and methods to allow the
// user to perform operations on it.
// ---
#ifndef FILE_H
#define FILE_H
#include <string>

class File
{
	protected:
		std::string _name;
		unsigned int _size;
		bool _isCut;

	public:
		File(const char*);

		//File operation functions:
		virtual File* cut();
		virtual File* copy();
		virtual bool paste(std::string);
		virtual bool deletef();
		virtual bool rename(std::string);

		//Getters:
		std::string getName();
		unsigned int getSize();
};

#endif
