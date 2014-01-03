#include "diskItem.h"
#include <sstream>
#include <cstdio>
#include <cctype>
#include <cmath>

void DiskItem::cut()
{
	_isCut = true;
}

bool DiskItem::rename(const char* newname)
{
	//Renames the file to 'newname', if it cannot, 
	//returns false:
	if(! std::rename(_path.c_str(), newname))
		return false;

	return true;
}

std::string DiskItem::getPath()
{
	return _path;
}

unsigned int DiskItem::getSize()
{
	return _size;
}

std::string DiskItem::getFormattedSize()
{
	//The formatted string:
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

bool byName(DiskItem* A, DiskItem* B)
{
	//Gives priority to dotfiles:
	if((A->getName()[0] == '.') && (B->getName()[0] == '.'))
	{
		if(lowercase(A->getName()) < lowercase(B->getName()))
			return true;
		else
			return false;
	}
	else if(A->getName()[0] == '.')
		return true;
	else if(B->getName()[0] == '.')
		return false;
	else if(lowercase(A->getName()) < lowercase(B->getName()))
		return true;
	else
		return false;
}

std::string lowercase(std::string s)
{
	std::string out = "";
	for(unsigned int i = 0; i < s.size(); i++)
		out += tolower(s[0]);
	return out;
}
