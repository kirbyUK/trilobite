#include "diskItem.h"
#include "directory.h"
#include "file.h"
#include <ncurses.h> 
#include <string>

struct windows
{
	WINDOW* window;
	int x, y, height, width;
} fileview, fileinfo, extrainfo;

//The help text at the bottom:
const std::string HELP_TEXT = "Z: Paste X: Cut C: Copy R: Rename D: Delete ?: Help";

//The height and width of the window:
int screenX = 0, screenY = 0;

//Draws the help text:
void drawHelp()
{
	init_pair(1, COLOR_WHITE, COLOR_BLUE);
	attron(COLOR_PAIR(1));
	mvprintw((screenY - 1), 0, "%s", HELP_TEXT.c_str());
	attroff(COLOR_PAIR(1));
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

int main()
{
	//Initialise ncurses:
	initscr();

	start_color();
	refresh();

	//Hides the cursor:
	curs_set(0);

	updateWindows();
	drawHelp();
	refresh();
	wrefresh(fileview.window);
	wrefresh(fileinfo.window);
	wrefresh(extrainfo.window);

	getch();

	//Close ncurses:
	endwin();
	return 0;
}
