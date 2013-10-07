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

		//Operation functions:
		void cut();
		virtual bool paste(std::string) = 0;
		virtual bool deletef() = 0;
		bool rename(const char*);

		//Getters:
		std::string getPath();
		virtual std::string getName() = 0;
		unsigned int getSize();
};

#endif
