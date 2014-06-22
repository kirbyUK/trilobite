// --- trilobite.cpp
#include "diskItem.h"
#include "directory.h"
#include "file.h"

#include <ncurses.h> 
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cerrno>
#include <cctype>
#include <unistd.h>

const short COLOUR = COLOR_BLUE; 

//Prints the given DiskItem's metadata to the fileinfo window:
void printMetaData(DiskItem*);

//Prints the passed clipboard's data:
void printClipboard(DiskItem*);

//Redraws all the windows:
void updateWindows();

//Draws the help text:
void drawHelp();

//Creates a message box with the passed error:
void messageBox(std::string);

//Creates an input box, allowing the user to enter
//text, which is returned:
std::string inputBox();

//Takes a directory path, and returns it shrunk to fit the size:
std::string fitToSize(std::string path, unsigned int size);

//Checks if the given character is allowed in a filename:
bool isValidInput(char c);

//The various windows used by the program:
struct windows
{
	WINDOW* window;
	unsigned int x, y, height, width;
} fileview, fileinfo, extrainfo, messagebox, inputbox;

//The help text at the bottom:
const std::string HELP_TEXT = " X: Cut C: Copy P: Paste R: Rename D: Delete Q: Quit";

//The height and width of the window:
unsigned int screenX = 0, screenY = 0;

int main(int argc, char* argv[])
{
	//The current working directory:
	Directory* dir = NULL;

	//Checks if too many arguments have been given:
	if(argc > 2)
	{
		std::cerr << "Please pass a single, valid directory\n";
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
		//Give an error message and quit if it fails:
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
		//Get the current working directory:
		char* cwd = getcwd(NULL, 0);

		//Attempt to open it:
		try
		{
			dir = new Directory(cwd);
		}
		//Give an error message and quit if it fails:
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
	//Attempt to read the directory's contents:
	try
	{
		dir->read();
	}
	//Give an error message and quit if it fails:
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

	if(dir->getName() == "../")
		dir->cleanPath();

	//Initialise ncurses:
	initscr();

	//The colour pairs:
	init_pair(2, COLOR_WHITE, COLOR_RED);
	//Enable keypad mode (allows use of the up and down arrows):
	keypad(stdscr, true);

	//Start using colour:
	start_color();

	//Update the screen:
	refresh();

	//Hides the cursor, set 'noecho()':
	curs_set(0);
	noecho();

	int input = 0;
	std::string path = "";
	unsigned int selection = 0;
	DiskItem* clipboard = NULL;

	//While the user has not quit:
	while((char(input) != 'q') && (char(input) != 'Q'))
	{
		//Redraw the windows and help:
		updateWindows();
		drawHelp();

		//If necessary, resizes the directory path:
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

		//Print the selected file's metadata to the 'fileinfo' window:
		printMetaData(items[selection + dir->getDotfiles()]);

		//Print the contents of the clipboard to the 'extrainfo' window:
		printClipboard(clipboard);

		//Refresh the screen and windows:
		refresh();
		wrefresh(fileview.window);
		wrefresh(fileinfo.window);
		wrefresh(extrainfo.window);

		//Gets the input:
		input = getch();

		//Moves the selection up or down if those keys were pressed:
		if((input == KEY_UP) || (char(input) == 'k') || (char(input) == 'K'))
			if(selection > 0) selection--;
		if((input == KEY_DOWN) || (char(input) == 'j') || (char(input) == 'J'))
			if(selection < ((items.size() - dir->getDotfiles()) - 1)) selection++;

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
				//If an error occurs, inform the user with a message box:
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
		//Otherwise, if the user has pressed 'd' for delete:
		else if((char(input) == 'd') || (char(input) == 'D'))
		{
			DiskItem* selected = items[selection + dir->getDotfiles()];
			//Attempt to delete the selected item:
			if(selected->deletef())
			{
				delete selected;
				dir->getFiles().erase(dir->getFiles().begin() + (selection + dir->getDotfiles()));

				//If we deleted the last item, then 'selection + dir->getDotfiles()' will
				//go out of bounds on the 'item' array, so decrement selection:
				if((selection + dir->getDotfiles()) == dir->getFiles().size())
					selection--;
			}
			//If an error occurs, inform the user with a message box:
			else
			{
				std::string error = "Could not delete '" + selected->getPath() + "'";
				messageBox(error);
			}
		}
		//Otherwise, if the user has pressed 'c' for copy:
		else if((char(input) == 'C') || (char(input) == 'c'))
		{
			if(items[selection + dir->getDotfiles()]->getName() != "../")
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
		}
		//Otherwise, if the user has pressed 'x' for cut:
		else if((char(input) == 'X') || (char(input) == 'x'))
		{
			if(items[selection + dir->getDotfiles()]->getName() != "../")
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
		}
		//Otherwise, if the user has pressed 'p' for paste:
		else if((char(input) == 'P') || (char(input) == 'p'))
		{
			if(clipboard != NULL)
			{
				//Attempt to paste the item in the clipboard:
				if(! clipboard->paste(dir->getPath()))
				{
					//If an error occurs, inform the user with a message box:
					std::string error = "Could not paste '" + clipboard->getName() + "'";
					messageBox(error);
				}
				else
				{
					//If it works fine, add the new item to the directory's list of items:
					DiskItem* item = clipboard;
					dir->getFiles().push_back(item);
					std::sort(dir->getFiles().begin(), dir->getFiles().end(), byName);

					//Empty the clipboard:
					clipboard = NULL;
				}
			}
		}
		//Otherwise, if the user presses 'r' for rename:
		else if((char(input) == 'R') || (char(input) == 'r'))
		{
			//Get the new name, and attempt to rename the selected item:
			std::string newName = inputBox();
			if(newName != "")
			{
				//Check if we are renaming a directory:
				if(dynamic_cast <Directory*>(items[selection + dir->getDotfiles()]) != NULL)
					newName += '/';

				if(! items[selection + dir->getDotfiles()]->rename(newName.c_str()))
				{
					//If an error occurs, inform the user with a message box:
					std::string error = "Cannot rename '" + items[selection + dir->getDotfiles()]->getName() + "'";
					messageBox(error);
				}
			}
		}
	}

	//Delete the directory object:
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
	//Check the clipboard isn't empty:
	if(clipboard != NULL)
	{
		//Gets the size of window, so we know how much we can print:
		unsigned int h = extrainfo.height - 2;
	
		//Print the title:
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

	//Creates new window objects with the new values:
	fileview.window = newwin(fileview.height, fileview.width, fileview.y, fileview.x);
	fileinfo.window = newwin(fileinfo.height, fileinfo.width, fileinfo.y, fileinfo.x);
	extrainfo.window = newwin(extrainfo.height, extrainfo.width, extrainfo.y, extrainfo.x);

	//Creates a border around each of the windows:
	wborder(fileview.window, '|', '|', '-', '-', '+', '+', '+', '+');
	wborder(fileinfo.window, '|', '|', '-', '-', '+', '+', '+', '+');
	wborder(extrainfo.window, '|', '|', '-', '-', '+', '+', '+', '+');
}

//Draws the help text:
void drawHelp()
{
	//Initialise the colour pair:
	init_pair(1, COLOR_WHITE, COLOUR);

	//Turn said pair on, and write the help text, then turn it off:
	attron(COLOR_PAIR(1));
	mvprintw((screenY - 1), 0, "%s", HELP_TEXT.c_str());
	attroff(COLOR_PAIR(1));
}

//Creates a message box with the given message and keeps it on screen:
void messageBox(std::string message)
{
	//Initialise the two colour pairs:
	init_pair(4, COLOR_WHITE, COLOUR);
	init_pair(5, COLOR_WHITE, COLOR_RED);

	//Resize the message box:
	messagebox.width = message.length() + 6;
	messagebox.height = screenY / 4;
	messagebox.x = ((screenX / 2) - (messagebox.width / 2));
	messagebox.y = ((screenY / 2) - (messagebox.height / 2));
	messagebox.window = newwin(messagebox.height, messagebox.width, messagebox.y, messagebox.x);

	//Set the background colour:
	wbkgd(messagebox.window, COLOR_PAIR(4));

	//Calculate the position of the text:
	unsigned int textX = 3;
	unsigned int textY = 2;

	//Calculate the position of the <OK> 'button':
	unsigned int buttonX = (messagebox.width / 2) - 2;
	unsigned int buttonY = messagebox.height - 2;

	//Write the text:
	mvwprintw(messagebox.window, textY, textX, "%s", message.c_str());

	//Write the <OK> 'button':
	wattron(messagebox.window, COLOR_PAIR(5));
	mvwprintw(messagebox.window, buttonY, buttonX, "%s", "<OK>");
	wattroff(messagebox.window, COLOR_PAIR(5));

	//Refresh the screen and the messagebox:
	refresh();
	wrefresh(messagebox.window);

	//Continuously get input, until the user presses the 'Enter' key:
	int input = 0;
	while(char(input) != '\n')
		input = getch();	

	//Clear the window:
	wclear(messagebox.window);
	clear();
}

//Creates an input box that allows the user to enter a string and returns it:
std::string inputBox()
{
	//Initialises the colour pairs:
	init_pair(4, COLOR_WHITE, COLOUR);
	init_pair(5, COLOR_WHITE, COLOR_RED);
	
	//Resize the input box:
	inputbox.width = screenX / 2;
	inputbox.height = screenY / 4;
	inputbox.x = ((screenX / 2) - (inputbox.width / 2));
	inputbox.y = ((screenY / 2) - (inputbox.height / 2));
	inputbox.window = newwin(inputbox.height, inputbox.width, inputbox.y, inputbox.x);

	//Set the background:
	wbkgd(inputbox.window, COLOR_PAIR(4));

	//The start position of the text entry box:
	unsigned int boxX = 2;
	unsigned int boxY = 2;
	unsigned int boxWidth = inputbox.width - 4;
	
	//The position of the buttons:
	unsigned int button1X = (inputbox.width / 4) - 2;
	unsigned int button2X = ((inputbox.width / 4) * 3) - 3;
	unsigned int buttonY = inputbox.height - 2;

	int input = 0, selection = 0;
	std::string inputStr = "";

	//The loop is indefinite, and ends when the user
	//presses '<OK>' or '<CANCEL>':
	while(1)
	{
		//Colours the text entry box red:
		mvwchgat(inputbox.window, boxX, boxY, boxWidth, A_NORMAL, 5, NULL);

		//Prints the text the user has entered:
		wattron(inputbox.window, COLOR_PAIR(5));
		mvwprintw(inputbox.window, boxY, boxX, "%s", inputStr.c_str());
		wattroff(inputbox.window, COLOR_PAIR(5));

		//Checks and highlights what area is currently selected:
		switch(selection)
		{
			//The text box:
			case 0: curs_set(1);
					mvwprintw(inputbox.window, buttonY, button1X, "%s", "<OK>");
					mvwprintw(inputbox.window, buttonY, button2X, "%s", "<CANCEL>");
					wmove(inputbox.window, boxY, (boxX + inputStr.size()));
					break;

			//The '<OK>' button:
			case 1: curs_set(0);
					wattron(inputbox.window, COLOR_PAIR(5));
					mvwprintw(inputbox.window, buttonY, button1X, "%s", "<OK>");
					wattroff(inputbox.window, COLOR_PAIR(5));
					mvwprintw(inputbox.window, buttonY, button2X, "%s", "<CANCEL>");
					break;

			//The '<CANCEL>' button:
			case 2: curs_set(0);
					wattron(inputbox.window, COLOR_PAIR(5));
					mvwprintw(inputbox.window, buttonY, button2X, "%s", "<CANCEL>");
					wattroff(inputbox.window, COLOR_PAIR(5));
					mvwprintw(inputbox.window, buttonY, button1X, "%s", "<OK>");
					break;
		}

		//Refreshes the inputbox:
		wrefresh(inputbox.window);

		//Gets the input to check for the Tab key:
		input = getch();

		//If the input is tab, change the selection:
		if(char(input) == '\t')
		{
			switch(selection)
			{
				case 0: selection = 1; break;
				case 1: selection = 2; break;
				case 2: selection = 0; break;
				default: selection = 0;
			}
		}
		//If the input is Enter, close the box and
		//return a value based on the selection:
		else if(char(input) == '\n')
		{
			switch(selection)
			{
				//The user has clicked '<OK>', return the
				//contents of the text box:
				case 1: wclear(inputbox.window);
						wrefresh(inputbox.window);
						if(inputStr.size() == 0) return NULL; else return inputStr; 
						break;

				//The user has clicked '<CANCEL>', return
				//nothing:
				case 2: wclear(inputbox.window);
						wrefresh(inputbox.window);
						return "";
						break;
			}
		}
		//If the user has pressed backspace, attempt to delete some of the input:
		else if((input == KEY_BACKSPACE) || (input == KEY_DC))
		{
			//If we're editing the input, backspace deletes a character off the end:
			if((selection == 0) && (inputStr.length() > 0))
			{
				inputStr.erase(inputStr.length() - 1);

				//Clear the box and redraw it:
				wclear(inputbox.window);
				wbkgd(inputbox.window, COLOR_PAIR(4));
			}
		}
		else
			//Otherwise, check if the input is an alphanumeric character, and if so,
			//add it to the end of our input string:
			if(isValidInput(input) && (selection == 0))
				inputStr += input;
	}
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

bool isValidInput(char c)
{
	if((isalnum(c)) || (c == '.') || (c == '-') || (c == '_'))
		return true;

	return false;
}
