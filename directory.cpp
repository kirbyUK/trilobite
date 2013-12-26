#include "directory.h"
#include "file.h"
#include <cerrno>
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
		throw errno;

	//Checks the passed file is a directory:
	if(S_ISDIR(_attr->st_mode) == 0)
	{
		errno = ENOTDIR;
		throw errno;
	}

	//Sets the directory path, adds a '/' if there is not one:
	_path = path;
	if(_path[_path.size() - 1] != '/')
		_path += '/';

	_isCut = false;
}

//Makes a copy of the passed DiskItem:
Directory::Directory(Directory* dir)
{
	_size = dir->getSize();
	_path = dir->getPath();
	_files = dir->getFiles();
	_isCut = false;

	//Reads the directory's attributes into '_attr':
	_attr = new struct stat;
	if(stat(_path.c_str(), _attr) != 0)
		throw errno;
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

//Reads the first layer of files and directories:
void Directory::read()
{

	//Creates a pointer to a 'DIR' struct:
	DIR* dir = opendir(_path.c_str());
	if(dir == NULL)
		throw errno;

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
			throw errno;

		if(S_ISDIR(attr->st_mode) != 0)
		{
			//Attempt to open the directory:
			try
			{
				file = new Directory(filepath.c_str());	
				file->calcSize();
			}
			catch(int errno)
			{
				throw errno;
			}
		}
		else
		{
			//Attempt to open the file:
			try
			{
				file = new File(filepath.c_str());
			}
			catch(int errno)
			{
				throw errno;
			}
		}
		//Delete the 'struct stat':
		delete attr;
	
		//Add the file (or directory) to the list of files:
		_files.push_back(file);

		//Read the next entry:
		dir_contents = readdir(dir);
	}
}

//Calculates the size of a directory:
void Directory::calcSize()
{
	_size = _attr->st_size;

	//Creates a pointer to a 'DIR' struct:
	DIR* dir = opendir(_path.c_str());
	if(dir == NULL)
		throw errno;

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

		//Checks if the path is a directory or a file:
		struct stat* attr = new struct stat;
		if(stat(filepath.c_str(), attr) != 0)
			throw errno;
		
		DiskItem* file = NULL;

		if(S_ISDIR(attr->st_mode) != 0)
		{
			//Attempt to open the directory:
			try
			{
				file = new Directory(filepath.c_str());	
				file->calcSize();
			}
			catch(int errno)
			{
				throw errno;
			}
			//Get the directory's size:
			_size += file->getSize();
		}
		else
		{
			//Attempt to open the file:
			try
			{
				file = new File(filepath.c_str());
			}
			catch(int errno)
			{
				throw errno;
			}
			//Get the file's size:
			_size += file->getSize();
		}
		dir_contents = readdir(dir);
		delete attr;
		delete file;
	}
}

bool Directory::paste(std::string newpath)
{
	//Creates a new directory in the new path:
	std::string path = newpath + getName();
	if(mkdir(path.c_str(), _attr->st_mode) != 0)
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
	int pos = _path.find_last_of('/', (_path.size() - 2));

	//Returns the substring from that position to the end
	//of the string, the directory name:
	return _path.substr(pos + 1);
}

std::vector <DiskItem*>& Directory::getFiles()
{
	return _files;
}
