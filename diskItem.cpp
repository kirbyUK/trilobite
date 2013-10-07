#include "diskItem.h"
#include <cstdio>

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
