#include "directory.h"
#include "file.h"
#include <cerrno>
#include <fstream>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <algorithm>

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
		{
			//Delete the 'struct stat':
			delete attr;

			//Read the next entry:
			dir_contents = readdir(dir);
			continue;
		}

		if(S_ISDIR(attr->st_mode) != 0)
		{
			//Attempt to open the directory:
			try
			{
				file = new Directory(filepath.c_str());	
				file->calcSize();
			}
			catch(int e)
			{
				//Delete the 'struct stat':
				delete attr;

				//Read the next entry:
				dir_contents = readdir(dir);
				continue;
			}
		}
		else
		{
			//Attempt to open the file:
			try
			{
				file = new File(filepath.c_str());
			}
			catch(int e)
			{
				//Delete the 'struct stat':
				delete attr;

				//Read the next entry:
				dir_contents = readdir(dir);
				continue;
			}
		}
		//Delete the 'struct stat':
		delete attr;
	
		//Add the file (or directory) to the list of files:
		_files.push_back(file);

		//Read the next entry:
		dir_contents = readdir(dir);
	}
	//Close the directory:
	closedir(dir);

	std::sort(_files.begin(), _files.end(), byName);

	//Count the dotfiles:
	_dotfiles = 0;
	for(unsigned int i = 0; i < _files.size(); i++)
		if(_files[i]->getName()[0] == '.')
			_dotfiles++;

	//Finally, add a link to the parent dir at the front:
	if(_path != "/")
	{
		DiskItem* parent = NULL;
		std::string path = _path + "../";
		try
		{
			parent = new Directory(path.c_str());
		}
		catch(int e)
		{
			throw e;
		}
		_files.insert(_files.begin() + _dotfiles, parent);
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
		{
				//Delete the 'struct stat':
				delete attr;

				//Read the next entry:
				dir_contents = readdir(dir);
				continue;
		}
		
		DiskItem* file = NULL;

		if(S_ISDIR(attr->st_mode) != 0)
		{
			//Attempt to open the directory:
			try
			{
				file = new Directory(filepath.c_str());	
				file->calcSize();
			}
			catch(int e)
			{
				throw e;
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
			catch(int e)
			{
				throw e;
			}
			//Get the file's size:
			_size += file->getSize();
		}
		dir_contents = readdir(dir);
		delete attr;
		delete file;
	}
	//Close the directory:
	closedir(dir);
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
	read();
	//Deletes the files and directories contained
	//in the directory, then deletes the directory:
	for(unsigned int i = 0; i < _files.size(); i++)
	{
		if(_files[i]->getName() != "../")
			if(! _files[i]->deletef())
				return false;
	}

	if(rmdir(_path.c_str()) != 0)
		return false;

	return true; 
}

//Say the directory path is '/home/alex/Code/trilobite/../'.
//This cleans it up so it's just '/home/alex/Code/'. It just takes
//out the last two items from the path, so be sure to check for '../'
//at the end before it is used:
void Directory::cleanPath()
{

	//Finds the 2nd to last '/':
	int pos1 = _path.find_last_of('/', (_path.size() - 2));

	//Using that, find the 3rd to last '/':
	int pos2 = _path.find_last_of('/', (pos1 - 1));

	_path = _path.substr(0, (pos2 + 1));
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

unsigned int Directory::getDotfiles()
{
	return _dotfiles;
}
