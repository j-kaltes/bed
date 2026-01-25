#include "screen.h"
#include "editor.h"
#include "cursesinit.h"
#include "misc.h"
/*DOESNT' WORK*/
int Editor::reorient(void) {
	resizeterm(thecols, thelines);
	curses_init_curses.resized();
/*
	myswap(thecols,thelines);
	curses_init_curses.setscreen();
*/
	}
