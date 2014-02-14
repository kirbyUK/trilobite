// ---
// diskItem.h
//
// Contains the class definition for a
// disk item, the parent class of the
// file and directory classes:
// ---

#ifndef DISK_ITEM_H
#define DISK_ITEM_H
#include <string>
#include <sys/stat.h>

class DiskItem
{
	protected:
		std::string _path;
		unsigned int _size;
		struct stat* _attr;
		bool _isCut;

	public:
		//Virtual destructor:
		virtual ~DiskItem() { }

		//Calculates the size of a directory:
		virtual void calcSize() = 0;

		//Operation functions:
		void cut();
		virtual bool paste(std::string) = 0;
		virtual bool deletef() = 0;
		bool rename(const char*);

		//Returns a string with the filesize and
		//an appropriate unit:
		std::string getFormattedSize();

		//Getters:
		std::string getPath();
		virtual std::string getName() = 0;
		unsigned int getSize();
};

//Checks the names of the two items passed,
//returns true if they are in order, or false
//if they are not. Used with the 'sort' function
//from the standard 'algorithm' library:
bool byName(DiskItem*, DiskItem*);

//Takes a string an returns the lowercase variant:
std::string lowercase(std::string);

#endif
