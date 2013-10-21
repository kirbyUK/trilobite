#include "diskItem.h"
#include <cstdio>
#include <cctype>

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

bool byName(DiskItem* A, DiskItem* B)
{
	if(lowercase(A->getName()) < lowercase(B->getName()))
		return true;
	else
		return false;
}

std::string lowercase(std::string s)
{
	std::string out = "";
	for(unsigned int i = 0; i < s.size(); i++)
		out += tolower(s[i]);
	return out;
}
