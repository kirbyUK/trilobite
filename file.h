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
		std::string _path;
		unsigned int _size;
		bool _isCut;

	public:
		File(const char*);

		//File operation functions:
		virtual std::string cut();
		virtual std::string copy();
		virtual bool paste(std::string, std::string);
		virtual bool deletef();
		virtual bool rename(const char*);

		//Getters:
		std::string getPath();
		std::string getName();
		unsigned int getSize();
};

#endif
