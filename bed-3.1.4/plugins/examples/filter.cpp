/*     Bed a menu-driven multi dataformat binary editor for Linux            */
/*     Copyright (C) 1998 Jaap Korthals Altes <binaryeditor@gmx.com>       */
/*                                                                           */
/*     This program is free software; you can redistribute it and/or modify  */
/*     it under the terms of the GNU General Public License as published by  */
/*     the Free Software Foundation; either version 2 of the License, or     */
/*     (at your option) any later version.                                   */
/*                                                                           */
/*     This program is distributed in the hope that it will be useful,       */
/*     but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*     GNU General Public License for more details.                          */
/*                                                                           */
/*     You should have received a copy of the GNU General Public License     */
/*     along with this program; if not, write to the Free Software           */
/*     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.             */
/* Fri Dec  8 22:14:27 2000                                                  */
#ifdef GLOBAL
#include <errno.h>
#include <stdlib.h>
       #include <sys/types.h>
              #include <sys/stat.h>
                     #include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>

     #include <sys/types.h>
     #include <unistd.h>
     #include <stdio.h>
     #include <stdlib.h>
     #include <fcntl.h>
       #include <sys/types.h>

              #include <signal.h>
#include <ctype.h>
#include "varmacro.h"


#define DEBUG(x) 

#endif



#ifdef EDITOR



#ifdef OLDGCCVARMACRO
#define	debugmessage(mes...) {message(mes);erefresh();}
#define fprintf(x,mes...) {message(mes);erefresh();}
#else
#define	debugmessage(...) {message(__VA_ARGS__);erefresh();}
#define fprintf(x,...) {message(__VA_ARGS__);erefresh();}
#endif


     /* Read characters from the pipe and echo them to `stdout'. */
//#define DEBUG(x) write(STDERR_FILENO,x,strlen(x));




int filter (void) ;

#endif


call(filter)
#ifdef UNDER
sethelp(filter,"Gives the content of the selection as input to a user specified command\n(e.g. grep '^X'|sort|uniq) and places the output of that command in the\ncopy buffer.")
int Editor::filter (void) {
	OFFTYPE other=getselect();
	if(other==OFFTYPEINVALID) {
		message("The data to be filtered should be selected!!");
		return -1;
		}
	char ant[400];
	inputget(ant ,"Command: ") ;/* put message and get answer in buffer answer (should be big enough) */
	toscreen();
	if(!ant[0]) {
		message("No command");
		return -1;
		}
	OFFTYPE len,first;
	if(!(len=getselregion(&first))) {
		message("The data to be filtered should be selected!!");
		return -1;
		}
	OFFTYPE max=2*len+20000,size;
	char buf[max];
	getmem(first,buf,len);
	char inname[]="/tmp/bed-in-XXXXXX";
	char outname[]="/tmp/bed-out-XXXXXX";
	mktemp(inname);
	mktemp(outname);
	int inhan=creat(inname,0777);
	if(inhan<0) {
		message("tmp error");return -1;
		}
	if((size=write(inhan,buf,len))<len) {
		message("Couldn't write everything!!");
		return -1;
		}
	close(inhan);
	char command[512];
	sprintf(command,"cat < %s|%s > %s",inname,ant,outname);
	debugmessage(command)
	if(system(command)<0) {
		message("%s failt",command);
		return -2;
		}
	int outhan=open(outname,O_RDONLY);
	if(outhan<0) {message("Cant open %s",outhan);return -1;};
	size=read(outhan,buf,max);
	if(size<0) {
		message("read error");return -1;
		}
	close(outhan);
	debugmessage("Read");
	//setselect(OFFTYPEINVALID);
	if(size>0) {	
		putselbuf(buf,size);
		message("Filtered data (#" OFFPRINT ") put in copy buffer. Past to use it",size);erefresh();
		}
	else {
		message("Generated no output",size);erefresh();
		}

	remove(outname);
	remove(inname);
	return 0;
	}
#endif
