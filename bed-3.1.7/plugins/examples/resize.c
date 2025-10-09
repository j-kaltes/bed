#include <pdcurses.h>
#include <stdlib.h>

void main(int argc,char **argv) {
	initscr();
	resize_term(atoi(argv[1]),atoi(argv[2]));
	system("c:/temp/bed.exe");
	endwin();
	}
