// --- file.cpp
#include "file.h"
#include <fstream>
#include <cstdio>
#include <sys/stat.h>

File::File(const char* path)
{
	//Reads the file's attributes into '_attr':
	_attr = new struct stat;
	if(stat(path, _attr) != 0)
		throw "I am error";

	//Checks the passed file is not a directory:
	if(S_ISDIR(_attr->st_mode) != 0)
		throw "Is a directory!";

	//'path' should be the full file path,
	//so we can open it for reading immediatly:
	//We open it in binary mode as we do not want
	//to parse the contents, simply read them and
	//get the size:
	std::ifstream in(path, std::ios::binary);
	
	//However, if there is an issue reading the file,
	//throw an error:
	if(! in)
		throw "I am error";

	//Get the filesize by going to the end of the file,
	//and storing the position:
	in.seekg(0, std::ifstream::end);
	_size = in.tellg();

	//Closes the file:
	in.close();

	//Saves the filename:
	_path = path;

	_isCut = false;
}

File::~File()
{
	//Deletes the stat struct:
	delete _attr;
}

std::string File::cut()
{
	//Read in the file:
	std::ifstream in(_path.c_str(), std::ios::binary);
		
	//The string the file will be read to:
	std::string file = "";

	if(! in)
		return NULL;
	else
	{
		_isCut = true;

		//Reads the file:
		while(in.good())
			file += char(in.get());

		in.close();
	}
	return file; 
}

std::string File::copy()
{
	//Read in the file:
	std::ifstream in(_path.c_str(), std::ios::binary);
	
	//The string the file will be read to:
	std::string file = "";

	if(! in)
		return NULL;
	else
	{
		//Reads the file:
		while(in.good())
			file += char(in.get());

		in.close();
	}
	return file; 
}

bool File::paste(std::string buffer, std::string newpath)
{
	//Opens an output file:
	std::string path = newpath + getName();
	std::ofstream out(path.c_str(), std::ios::binary);

	if(! out)
		return false;

	//Writes the file:
	out << buffer;

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

bool File::rename(const char* newname)
{
	//Renames the file to 'newname', if it cannot, 
	//returns false:
	if(! std::rename(_path.c_str(), newname))
		return false;

	return true;
}

std::string File::getPath()
{
	return _path;
}

std::string File::getName()
{
	//Finds the position of the last '/':
	int pos = _path.find_last_of('/');

	//Returns the substring from that position
	//to the end of the string, the filename:
	return _path.substr(pos + 1);
}

unsigned int File::getSize()
{
	return _size;
}
