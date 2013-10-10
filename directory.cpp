#include "directory.h"
#include "file.h"
#include <fstream>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>

Directory::Directory(const char* path)
{
	//Reads the directory's attributes into '_attr':
	_attr = new struct stat;
	if(stat(path, _attr) != 0)
		throw "I am error";

	//Checks the passed file is a directory:
	if(S_ISDIR(_attr->st_mode) == 0)
		throw "Is a not directory!";

	//Sets the directory path, adds a '/' if there is not one:
	_path = path;
	if(_path[_path.size() - 1] != '/')
		_path += '/';

	//Creates a pointer to a 'DIR' struct:
	DIR* dir = opendir(path);
	if(dir == NULL)
		throw "I am error";

	//Creates a pointer to a 'dirent' struct:
	dirent* dir_contents = readdir(dir);

	//While there is stuff to read:
	while(dir_contents != 0)
	{
		std::string name = dir_contents->d_name;

		//Checks we are not reading ".":
		if((name == ".") || (name == ".."))
		{
			//Read the next entry:
			dir_contents = readdir(dir);
			continue;
		}

		//The full path of the file:
		std::string filepath = _path + name;

		DiskItem* file = NULL;

		//Checks if the path is a directory or a file:
		struct stat* attr = new struct stat;
		if(stat(filepath.c_str(), attr) != 0)
			throw "I am error";
		
		if(S_ISDIR(attr->st_mode) != 0)
		{
			//Attempt to open the directory:
			try
			{
				file = new Directory(filepath.c_str());	
			}
			catch(const char* e)
			{
				throw e;
			}
		}
		else
		{
			//Attempt to open the file:
			try
			{
				file = new File(filepath.c_str());
			}
			catch(const char* e)
			{
				throw e;
			}
		}
		//Delete the 'struct stat':
		delete attr;
	
		//Add the file (or directory) to the list of files:
		_files.push_back(file);

		//Read the next entry:
		dir_contents = readdir(dir);
	}

	_size = _attr->st_size;

	//Gets the size of the directory by adding the size of
	//the contents together:
	for(unsigned int i = 0; i < _files.size(); i++)
		_size += _files[i]->getSize();	

	_isCut = false;
}

Directory::~Directory()
{
	//Deletes all the file and directory objects
	//associated with the directory:
	for(unsigned int i = 0; i < _files.size(); i++)
		delete _files[i];

	//Deletes the directory attributes:
	delete _attr;
}

bool Directory::paste(std::string newpath)
{
	//Creates a new directory in the new path:
	std::string path = newpath + getName();
	if(mkdir(newpath.c_str(), _attr->st_mode) != 0)
		return false;

	//Copies the contents of the directory to
	//the newly created directory:
	for(unsigned int i = 0; i < _files.size(); i++)
		if(! _files[i]->paste(path))
			return false;

	//If the file was set to cut, delete the contents
	//and then delete the directory:
	if(_isCut)
		if(! deletef())
			return false;

	return true;
}

bool Directory::deletef()
{
	//Deletes the files and directories contained
	//in the directory, then deletes the directory:
	for(unsigned int i = 0; i < _files.size(); i++)
		if(! _files[i]->deletef())
			return false;

	if(rmdir(_path.c_str()) != 0)
		return false;

	return true; 
}

std::string Directory::getName()
{
	//Gets the position of the second to last '/', as
	//we know the very last character will be '/':
	int pos = _path.find_last_of('/', (_path.size() - 1));

	//Returns the substring from that position to the end
	//of the string, the directory name:
	return _path.substr(pos + 1);
}

std::vector <DiskItem*>& Directory::getFiles()
{
	return _files;
}
