// --- file.cpp
#include "file.h"
#include <fstream>
#include <cstdio>

File::File(char* path)
{
	//'path' should be the full file path,
	//so we can open it for reading immediatly:
	//We open it in binary mode as we do not want
	//to parse the contents, simply read them and
	//get the size:
	ifstream in(path, std::ios::binary);
	
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
	_name = path;

	_isCut = false;
}

File* File::cut()
{
	_isCut = true;
	return this;
}

File* File::copy()
{
	return this;
}

bool File::paste(std::string newpath)
{
	//Read in the file to paste:
	ifstream in(_name, std::ios::binary);
	
	if(! in)
		return false;

	//The string the file will be read to:
	std::string file = "";

	//Reads the file:
	while(in.good())
		file += char(in.get());

	in.close();

	//Opens an output file:
	ofstream out(newpath.c_str(), std::ios::binary);

	if(! out)
		return false;

	//Writes the file:
	out << file;

	out.close();

	//If we are cutting the file, delete the original:
	if(_isCut)
		if(! deletef(_name))
			return false;

	return true;
}

bool File::deletef()
{
	//Removes the file, if it cannot, returns false:
	if(! remove(_name.c_str()))
		return false;

	return true;
}

bool File::rename(std::string newname)
{
	//Renames the file to 'newname', if it cannot, 
	//returns false:
	if(! rename(_file, newname))
		return false;

	return true;
}

std::string File::getName()
{
	return _name;
}

unsigned int File::getSize()
{
	return _size;
}
