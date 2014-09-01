// --- diskItem.cpp
#include "diskItem.h"
#include <sstream>
#include <cstdio>
#include <cctype>
#include <cmath>

//Marks the DiskItem as cut:
void DiskItem::cut()
{
	_isCut = true;
}

bool DiskItem::rename(const char* newname)
{
	std::string newname2 = newname;

	std::string name = getName();
	std::string newPath = (_path.substr(0, (_path.length() - name.length()))) + newname2;

	//Renames the file to 'newname', if it cannot, returns false:
	if(std::rename(_path.c_str(), newPath.c_str()) != 0)
		return false;

	_path = newPath; 

	return true;
}

//Returns the path:
std::string DiskItem::getPath()
{
	return _path;
}

//Returns the filesize:
unsigned int DiskItem::getSize()
{
	return _size;
}
 
std::string DiskItem::getFormattedSize()
{
	//The formatted string, set to use zero
	//decimal places and 'fixed' notation (as
	//opposed to scientific):
	std::stringstream formatted;
	formatted.precision(0);
	formatted.setf(std::ios::fixed);

	//Checks if the size is in bytes:
	if((_size / pow(2, 10)) < 1)
	{
		formatted << _size << "B";
	}
	//Checks if the size is in kilobytes:
	else if((_size / pow(2, 20)) < 1)
	{
		float newSize = (_size / pow(2, 10));
		formatted << newSize << "kB";
	}
	//Checks if the size is in megabytes:
	else if((_size / pow(2, 30)) < 1)
	{
		float newSize = (_size / pow(2, 20));
		formatted << newSize << "MB";
	}
	//Checks if the size is in gigabytes:
	else if((_size / pow(2, 40)) < 1)
	{
		float newSize = (_size / pow(2, 30));
		formatted << newSize << "GB";
	}
	//Checks if the size is in terabytes:
	else if((_size / pow(2, 50)) < 1)
	{
		float newSize = (_size / pow(2, 40));
		formatted << newSize << "TB";
	}
	return formatted.str();
}

//Sorts DiskItems by name, giving priority to dotfiles:
bool byName(DiskItem* A, DiskItem* B)
{
	std::string a = A->getName();
	std::string b = B->getName();

	//Check if either file is a dotfile:
	if((a[0] == '.') && (b[0] == '.'))
	{
		//If they both are, sort by name:
		if(lowercase(a) < lowercase(a))
			return true;
		else
			return false;
	}
	//If only one is a dotfile, it gets priority:
	else if(a[0] == '.')
		return true;
	else if(b[0] == '.')
		return false;
	//Otherwise, neither is a dotfile, so sort by name:
	else if(lowercase(a) < lowercase(b))
		return true;
	else
		return false;
}

std::string lowercase(std::string s)
{
	//Loops through the string, making each character lowercase
	//and appending it to the output string, which is returned:
	std::string out = "";
	for(unsigned int i = 0; i < s.size(); i++)
		out += tolower(s[i]);
	return out;
}
