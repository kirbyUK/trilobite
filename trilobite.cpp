#include "diskItem.h"
#include "directory.h"
#include "file.h"

#include <ncurses.h> 
#include <iostream>
#include <string>
#include <cerrno>
#include <unistd.h>

void updateWindows();
void drawHelp();
std::string fitToSize(std::string path, unsigned int size);

struct windows
{
	WINDOW* window;
	int x, y, height, width;
} fileview, fileinfo, extrainfo;

//The help text at the bottom:
const std::string HELP_TEXT = " X: Cut C: Copy V: Paste R: Rename D: Delete ?: Help";

//The height and width of the window:
unsigned int screenX = 0, screenY = 0;

int main(int argc, char* argv[])
{
	//The current working directory:
	Directory* currentDir = NULL;

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
			currentDir = new Directory(argv[1]);
		}
		catch(int e)
		{
			std::cerr << "Cannot open '" << argv[1] << "': ";
			switch(errno)
			{
				case EACCES:  std::cerr << "Permission denied.\n"; break;
				case ENOENT:  std::cerr << "No such directory.\n"; break;
				case ENOTDIR: std::cerr << "Not a directory.\n"; break;
			}
			return -1;
		}
	}
	//Otherwise, there are no arguments given, so use
	//the user's current working directory:
	else
	{
		try
		{
			char* cwd = get_current_dir_name();
			currentDir = new Directory(cwd);
		}
		catch(int e)
		{
			std::cerr << "Cannot open '" << argv[1] << "': ";
			switch(errno)
			{
				case EACCES:  std::cerr << "Permission denied.\n"; break;
				case ENOENT:  std::cerr << "No such directory.\n"; break;
				case ENOTDIR: std::cerr << "Not a directory.\n"; break;
			}
			return -1;
		}
	}

	//Initialise ncurses:
	initscr();

	start_color();
	refresh();

	//Hides the cursor:
	curs_set(0);

	updateWindows();
	drawHelp();

	//If nessecary, resizes the directory path:
	std::string path = "";
	if(currentDir->getPath().length() >= screenX)
		path = fitToSize(currentDir->getPath(), (screenX - 2));
	else
		path = currentDir->getPath();


	//The X position needed to print the path in the centre:
	int pos = ((screenX - path.length()) / 2);
	//Write the user's current directory:
	mvprintw(0, pos, "%s", path.c_str());

	refresh();
	wrefresh(fileview.window);
	wrefresh(fileinfo.window);
	wrefresh(extrainfo.window);

	getch();

	//Close ncurses:
	endwin();
	return 0;
}

//Updates the window size:
void updateWindows()
{
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
