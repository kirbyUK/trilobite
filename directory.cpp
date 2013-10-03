#include "directory.h"
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

	//Creates a pointer to a 'DIR' struct:
	DIR* dir = opendir(path);
	if(dir == NULL)
		throw "I am error";

	//Creates a pointer to a 'dirent' struct:
	dirent* dir_contents = readdir(dir);

	//While there is stuff to read:
	while(dir_contents != 0)
	{
		//The full path of the file:
		std::string filepath = path + dir_contents -> d_name;

		File* file = NULL;

		//Checks if the path is a directory or a file:
		struct stat* attr = new struct stat;
		if(stat(filepath, attr) != 0)
			throw "I am error";
		
		if(S_ISDIR(attr->st_mode) == 0)
		{
			//Appends a '/' to the filepath:
			filepath += '/';

			//Attempt to open the directory:
			try
			{
				file = new Directory(filepath);	
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

	//Gets the size of the directory by adding the size of
	//the contents together:
	for(unsigned int i = 0; i < _files.size(); i++)
		_size += _files[i]->getSize();	
}

Directory::~Directory()
{
	//Deletes all the file and directory objects
	//associated with the directory:
	for(unsigned int i = 0; i < _files.size(); i++)
		delete _files[i];
}

bool Directory::paste(std::string newpath)
{
	//Creates a new directory in the new path:
	path = newpath + getName();
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

	if(rmdir(_path) != 0)
		return false;

	return true; 
}

std::vector <File*>& Directory::getFiles()
{
	return _files;
}
