#include "diskItem.h"
#include "directory.h"
#include "file.h"

#include <ncurses.h> 
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cerrno>
#include <unistd.h>

//Prints the given DiskItem's metadata to the fileinfo window:
void printMetaData(DiskItem*);

//Prints the passed clipboard's data:
void printClipboard(DiskItem*);

void updateWindows();
void drawHelp();
void messageBox(std::string);

//Takes a directory path, and returns it shrunk to fit the size:
std::string fitToSize(std::string path, unsigned int size);

struct windows
{
	WINDOW* window;
	unsigned int x, y, height, width;
} fileview, fileinfo, extrainfo, messagebox;

//The help text at the bottom:
const std::string HELP_TEXT = " X: Cut C: Copy V: Paste R: Rename D: Delete Q: Quit";

//The height and width of the window:
unsigned int screenX = 0, screenY = 0;

int main(int argc, char* argv[])
{
	//The current working directory:
	Directory* dir = NULL;

	//Checks if too many arguments have been given:
	if(argc > 2)
	{
		std::cerr << "Please specify a directory.\n";
		return -1;
	}
	//Otherwise, sees if a directory has been given,
	//and if so, attempt to open it:
	else if(argc == 2)
	{
		//Attempt to open 
		try
		{
			dir = new Directory(argv[1]);
		}
		catch(int e)
		{
			std::cerr << "Cannot open '" << argv[1] << "': ";
			switch(e)
			{
				case EACCES:  std::cerr << "Permission denied."; break;
				case ENOENT:  std::cerr << "No such directory."; break;
				case ENOTDIR: std::cerr << "Not a directory."; break;
			}
			std::cerr << std::endl;
			return -1;
		}
	}
	//Otherwise, there are no arguments given, so use
	//the user's current working directory:
	else
	{
		char* cwd = get_current_dir_name();
		try
		{
			dir = new Directory(cwd);
		}
		catch(int e)
		{
			std::cerr << "Cannot open '" << cwd << "': ";
			switch(e)
			{
				case EACCES:  std::cerr << "Permission denied."; break;
				case ENOENT:  std::cerr << "No such directory."; break;
				case ENOTDIR: std::cerr << "Not a directory."; break;
			}
			std::cerr << std::endl;
			return -1;
		}
	}
	try
	{
		dir->read();
	}
	catch(int e)
	{
		std::cerr << "Cannot open '" << dir->getPath() << "': ";
		switch(errno)
		{
			case EACCES:  std::cerr << "Permission denied."; break;
			case ENOENT:  std::cerr << "No such directory."; break;
			case ENOTDIR: std::cerr << "Not a directory."; break;
		}
		std::cerr << std::endl;
		return -1;
	}

	//Initialise ncurses:
	initscr();

	//The colour pairs:
	init_pair(2, COLOR_WHITE, COLOR_RED);
	//Allows use of the up and down arrows:
	keypad(stdscr, true);

	start_color();
	refresh();

	//Hides the cursor:
	curs_set(0);
	noecho();

	int input = 0;
	std::string path = "";
	unsigned int selection = 0;
	DiskItem* clipboard = NULL;
	while((char(input) != 'q') && (char(input) != 'Q'))
	{
		updateWindows();
		drawHelp();

		//Cleans the path is nessecary:
		if(dir->getName() == "../")
			dir->cleanPath();

		//If nessecary, resizes the directory path:
		path = "";
		if(dir->getPath().length() >= screenX)
			path = fitToSize(dir->getPath(), (screenX - 2));
		else
			path = dir->getPath();

		//The X position needed to print the path in the centre:
		int pos = ((screenX - path.length()) / 2);
		//Write the user's current directory:
		mvprintw(0, pos, "%s", path.c_str());

		//Get the files and sort the contents:
		std::vector <DiskItem*> items = dir->getFiles();

		//Checks if the contents of the directory will fit in the window:
		if((fileview.height - 2) > (items.size() - dir->getDotfiles()))
		{
			//Print the contents, except the dotfiles, to the window:
			for(unsigned int i = dir->getDotfiles(); i < items.size(); i++)
			{
				//If we're printing the current selection, highlight it:
				if(selection == (i - dir->getDotfiles()))
				{
					//Print the name:
					mvwprintw(fileview.window,((i - dir->getDotfiles()) + 1), 1, "%s", items[i]->getName().c_str());

					//Move to the beginning of the line, and highlight the line up to but excluding the window border:
					mvwchgat(fileview.window, ((i - dir->getDotfiles()) + 1), 1, (fileview.width - 2), A_NORMAL, 1, NULL);
				}
				else
					//Print the name:
					mvwprintw(fileview.window, ((i - dir->getDotfiles()) + 1), 1, "%s", items[i]->getName().c_str());
			}
		}
		//Otherwise, we can only print part of the directory's contents:
		else
		{
			//If the selection is less than the height, display the first few items:
			if(selection < (fileview.height - 2))
			{
				for(unsigned int i = dir->getDotfiles(); i < ((fileview.height - 2) + dir->getDotfiles()); i++)
				{
					//If we're printing the current selection, highlight it:
					if(selection == (i - dir->getDotfiles()))
					{
						//Print the name:
						mvwprintw(fileview.window,((i - dir->getDotfiles()) + 1), 1, "%s", items[i]->getName().c_str());

						//Move to the beginning of the line, and highlight the line up to but excluding the window border:
						mvwchgat(fileview.window, ((i - dir->getDotfiles()) + 1), 1, (fileview.width - 2), A_NORMAL, 1, NULL);
					}
					else
						//Print the name:
						mvwprintw(fileview.window, ((i - dir->getDotfiles()) + 1), 1, "%s", items[i]->getName().c_str());
				}
			}
			//Otherwise, display the selection as the last item:
			else
			{
				for(unsigned int i = (dir->getDotfiles() + ((selection + 1) - (fileview.height - 2))); i < ((selection + 1) + dir->getDotfiles()); i++)
				{
					unsigned int y = i - ((selection - (fileview.height - 2)) + dir->getDotfiles());

					//If we're printing the current selection, highlight it:
					if(selection == (i - dir->getDotfiles()))
					{
						//Print the name:
						mvwprintw(fileview.window, y, 1, "%s", items[i]->getName().c_str());

						//Move to the beginning of the line, and highlight the line up to but excluding the window border:
						mvwchgat(fileview.window, y, 1, (fileview.width - 2), A_NORMAL, 1, NULL);
					}
					else
						//Print the name:
						mvwprintw(fileview.window, y, 1, "%s", items[i]->getName().c_str());
				}
			}
		}

		printMetaData(items[selection + dir->getDotfiles()]);
		printClipboard(clipboard);

		refresh();
		wrefresh(fileview.window);
		wrefresh(fileinfo.window);
		wrefresh(extrainfo.window);

		//Gets the input:
		input = getch();

		//Moves the selection up or down if those keys were pressed:
		switch(input)
		{
			case KEY_UP: 	if(selection > 0) selection--; break;
			case KEY_DOWN: 	if(selection < ((items.size() - dir->getDotfiles()) - 1)) selection++; break;
		}
		//If the user has pressed Enter:
		if(char(input) == '\n')
		{
			//Attempts to cast the current selection to a Directory*:
			Directory* selected = dynamic_cast <Directory*>(items[selection + dir->getDotfiles()]);

			//If the user has selected a directory:
			if(selected != NULL)
			{
				//Keep the old directory so we can delete it:
				Directory* oldDir = dir;

				//Makes a copy of the directory we want to move to:
				try
				{
					dir = new Directory(selected);
					dir->read();

					delete oldDir;
					selection = 0;

					clear();
				}
				catch(int e)
				{
					std::string error = "Cannot open '" + dir->getPath() + "' ";
					switch(errno)
					{
						case EACCES:  error += "Permission denied."; break;
						case ENOENT:  error += "No such directory."; break;
						case ENOTDIR: error += "Not a directory."; break;
					}
					messageBox(error);
				}
			}
		}
		else if((char(input) == 'd') || (char(input) == 'D'))
		{
			DiskItem* selected = items[selection + dir->getDotfiles()];
			if(selected->deletef())
			{
				delete selected;
				dir->getFiles().erase(dir->getFiles().begin() + (selection + dir->getDotfiles()));
			}
			else
			{
				std::string error = "Could not delete '" + selected->getPath() + "'";
				messageBox(error);
			}
		}
		else if((char(input) == 'C') || (char(input) == 'c'))
		{
			//Checks if we are trying to copy a directory or a file:
			clipboard = dynamic_cast <Directory*>(items[selection + dir->getDotfiles()]);

			if(clipboard == NULL)
			{
				//We are copying a file:
				clipboard = new File(dynamic_cast <File*>(items[selection + dir->getDotfiles()]));
			}
			else
			{
				//We are copying a directory:
				clipboard = new Directory(dynamic_cast <Directory*>(items[selection + dir->getDotfiles()]));
			}
		}
		else if((char(input) == 'X') || (char(input) == 'x'))
		{
			//Checks if we are trying to cut a directory or a file:
			clipboard = dynamic_cast <Directory*>(items[selection + dir->getDotfiles()]);

			if(clipboard == NULL)
			{
				//We are cutting a file:
				clipboard = new File(dynamic_cast <File*>(items[selection + dir->getDotfiles()]));
			}
			else
			{
				//We are cutting a directory:
				clipboard = new Directory(dynamic_cast <Directory*>(items[selection + dir->getDotfiles()]));
			}
			clipboard->cut();
		}
		else if((char(input) == 'P') || (char(input) == 'p'))
		{
			if(clipboard != NULL)
			{
				if(! clipboard->paste(dir->getPath()))
				{
					std::string error = "Could not paste '" + clipboard->getName() + "'";
					messageBox(error);
				}
				else
				{
					DiskItem* item = clipboard;
					dir->getFiles().push_back(item);
					std::sort(dir->getFiles().begin(), dir->getFiles().end(), byName);
					clipboard = NULL;
				}
			}
		}
	}

	delete dir;

	//Close ncurses:
	endwin();
	return 0;
}

//Prints the given DiskItem's metadata to the fileinfo window:
void printMetaData(DiskItem* item)
{
	//Gets the size of window, so we know how much we can print:
	unsigned int h = fileinfo.height - 2;
	
	//Print the name:
	if(h > 0) mvwprintw(fileinfo.window, 1, 1, "%s", item->getName().c_str());

	//Print if it is a file or directory:
	if(item->getName()[item->getName().size() - 1] == '/')
	{
		if(h > 1)
			mvwprintw(fileinfo.window, 2, 1, "%s", "Directory");
	}
	else
	{
		if(h > 1)
			mvwprintw(fileinfo.window, 2, 1, "%s", "File");
	}

	//Print the filesize:
	if((h > 2) && (item->getName() != "../"))
		mvwprintw(fileinfo.window, 3, 1, "%s", item->getFormattedSize().c_str());
}

//Prints the given DiskItem's metadata to the extrainfo window:
void printClipboard(DiskItem* clipboard)
{
	if(clipboard != NULL)
	{
		//Gets the size of window, so we know how much we can print:
		unsigned int h = extrainfo.height - 2;
	
		if(h > 0) mvwprintw(extrainfo.window, 1, 1, "%s", "CLIPBOARD:");

		//Print the name:
		if(h > 1) mvwprintw(extrainfo.window, 2, 1, "%s", clipboard->getName().c_str());

		//Print if it is a file or directory:
		if(clipboard->getName()[clipboard->getName().size() - 1] == '/')
		{
			if(h > 2)
				mvwprintw(extrainfo.window, 3, 1, "%s", "Directory");
		}
		else
		{
			if(h > 2)
				mvwprintw(extrainfo.window, 3, 1, "%s", "File");
		}

		//Print the filesize:
		if((h > 3) && (clipboard->getName() != "../"))
			mvwprintw(extrainfo.window, 4, 1, "%s", clipboard->getFormattedSize().c_str());
	}
}

//Updates the window size:
void updateWindows()
{
	//Clears the windows:
	wclear(fileview.window);
	wclear(fileinfo.window);
	wclear(extrainfo.window);

	//Gets the screen size:
	getmaxyx(stdscr, screenY, screenX);

	//Initialises the windows:
	fileview.x = 0; fileview.y = 1;
	fileview.width = screenX * 0.75;
	fileview.height = screenY - 2;

	fileinfo.x = fileview.width + 1;
	fileinfo.y = 1;
	fileinfo.width = (screenX - fileview.width) - 1;
	fileinfo.height = ((screenY - 2) * 0.75) - 1;

	extrainfo.x = fileinfo.x;
	extrainfo.y = fileinfo.y + fileinfo.height;
	extrainfo.width = fileinfo.width;
	extrainfo.height = (screenY - fileinfo.height) - 2;

	fileview.window = newwin(fileview.height, fileview.width, fileview.y, fileview.x);
	fileinfo.window = newwin(fileinfo.height, fileinfo.width, fileinfo.y, fileinfo.x);
	extrainfo.window = newwin(extrainfo.height, extrainfo.width, extrainfo.y, extrainfo.x);

	wborder(fileview.window, '|', '|', '-', '-', '+', '+', '+', '+');
	wborder(fileinfo.window, '|', '|', '-', '-', '+', '+', '+', '+');
	wborder(extrainfo.window, '|', '|', '-', '-', '+', '+', '+', '+');
}

//Draws the help text:
void drawHelp()
{
	init_pair(1, COLOR_WHITE, COLOR_BLUE);
	attron(COLOR_PAIR(1));
	mvprintw((screenY - 1), 0, "%s", HELP_TEXT.c_str());
	attroff(COLOR_PAIR(1));
}

//Creates a message box with the given message and keeps it on screen:
void messageBox(std::string message)
{
	init_pair(4, COLOR_WHITE, COLOR_BLUE);
	init_pair(5, COLOR_WHITE, COLOR_RED);

	//Resize the message box:
	messagebox.width = message.length() + 6;
	messagebox.height = screenY / 4;
	messagebox.x = ((screenX / 2) - (messagebox.width / 2));
	messagebox.y = ((screenY / 2) - (messagebox.height / 2));
	messagebox.window = newwin(messagebox.height, messagebox.width, messagebox.y, messagebox.x);

	//Set the background:
	wbkgd(messagebox.window, COLOR_PAIR(4));

	//Calculate the position of the text:
	unsigned int textX = 3;
	unsigned int textY = 2;

	//Calculate the poistion of the <OK> 'button':
	unsigned int buttonX = (messagebox.width / 2) - 2;
	unsigned int buttonY = messagebox.height - 2;

	//Write the text:
	mvwprintw(messagebox.window, textY, textX, "%s", message.c_str());

	//Write the <OK> 'button':
	wattron(messagebox.window, COLOR_PAIR(5));
	mvwprintw(messagebox.window, buttonY, buttonX, "%s", "<OK>");
	wattroff(messagebox.window, COLOR_PAIR(5));

	refresh();
	wrefresh(messagebox.window);

	int input = 0;
	while(char(input) != '\n')
		input = getch();	

	//Clear the window:
	wclear(messagebox.window);
	clear();
}

//Takes a directory path and returns it shrunk to the given size or smaller:
std::string fitToSize(std::string path, unsigned int size)
{
	//The position in the string, starts at 1 to skip the first '/':
	int pos = 1;
	while(path.length() > size)
	{
		//Gets the directory name between the '/':
		int newPos = path.find('/', pos);
		std::string dir = path.substr(pos, (newPos - pos));

		//The characters removed, minus 1 for the '/':
		int removed = dir.length() - 2;

		//Replace the directory name with the first letter:
		dir = dir[0];

		//Grabs the bit of the path before and after the edit to build the string:
		std::string front = path.substr(0, pos);
		std::string back = path.substr(newPos, (path.size() - newPos));
		path = (front + dir + back);

		pos = newPos - removed;
	}
	return path;
}
