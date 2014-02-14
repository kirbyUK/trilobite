// --- file.cpp
#include "file.h"
#include <fstream>
#include <cstdio>
#include <cerrno>
#include <sys/stat.h>

File::File(const char* path)
{
	//Reads the file's attributes into '_attr':
	_attr = new struct stat;
	if(stat(path, _attr) != 0)
		throw errno;

	//Checks the passed file is not a directory:
	if(S_ISDIR(_attr->st_mode) != 0)
		throw errno;

	//Gets the size:
	_size = _attr->st_size;

	//Saves the filename:
	_path = path;

	_isCut = false;
}

File::File(File* file)
{
	//Copies the passed file's data:
	_size = file->getSize();
	_path = file->getPath();
	_isCut = false;

	//Reads the file's attributes into '_attr':
	_attr = new struct stat;
	if(stat(_path.c_str(), _attr) != 0)
		throw errno;
}

File::~File()
{
	//Deletes the stat struct:
	delete _attr;
}

//Creates a copy of the file in the passed location:
bool File::paste(std::string newpath)
{
	//Opens an input file:
	std::ifstream in(_path.c_str(), std::ios::binary);

	//Opens an output file:
	std::string path = newpath + getName();
	std::ofstream out(path.c_str(), std::ios::binary);

	//If either cannot be opened:
	if((! in) || (! out))
		return false;

	//Writes the input file to the output file:
	out << in.rdbuf();

	//Closes the files:
	in.close();
	out.close();

	//If we are cutting the file, delete the original:
	if(_isCut)
		if(! deletef())
			return false;

	//Get the original's permission bits:
	mode_t permission = _attr->st_mode;

	//Attempts to write the original permission bits:
	if(chmod(path.c_str(), permission) != 0)
		return false;

	return true;
}

bool File::deletef()
{
	//Removes the file, if it cannot, returns false:
	if(remove(_path.c_str()) != 0)
		return false;

	return true;
}

std::string File::getName()
{
	//Finds the position of the last '/':
	int pos = _path.find_last_of('/');

	//Returns the substring from that position
	//to the end of the string, the filename:
	return _path.substr(pos + 1);
}
