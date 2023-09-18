#include "defines.h"
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

#include <features.h>
extern int starttrace(void) ;
extern int	traceproc(Editor* ed, int (Editor::*pr)(void)) ;
extern int endtrace(void) ;
FILE *tracefile;
int tracing=1;
#include <stdio.h>
#include <time.h>
#include <unistd.h>
       #include <pwd.h>
              #include <sys/types.h>


int starttrace(void) {
	time_t tim;
	char tracename[256];
	char userbuf[]="";
	char *user=userbuf;
	struct passwd *pas;
	if((pas=getpwuid(getuid())))
		user=pas->pw_name;
	snprintf(tracename,256,"/tmp/bedtrace_%s.%d",user,pid);
	if(!tracing)
		return -1;
	tracefile=fopen(tracename, "a");
	setvbuf(tracefile,NULL,  _IONBF,0);
	time(&tim);
	fprintf(tracefile,"\nRestart %d at %s",pid,ctime(&tim));
	return 0;
	}

int	traceproc(Editor* ed, int (Editor::*pr)(void)) {
	int i;
	for(i=0;i<Editor::procnr;i++) {
		if(Editor::proc[i]==pr) {
			fprintf(tracefile,"%-55.55s: %s\n",ed->mem.filename,Editor::procnames[i]);
			fflush(tracefile);
			return 0;
			}
		}
	for(i=0;i<Editor::dynprocnr;i++) {
		if(Editor::dynproc[i]==pr) {
			fprintf(tracefile,"%-55.55s: %s\n",ed->mem.filename,Editor::dynprocnames[i]);
			fflush(tracefile);
			return 0;
			}
		}
	fprintf(tracefile,"%-55.55s: Unknown procedure (ERROR)\n",ed->mem.filename);
	fflush(tracefile);
	return -1;
	}


int endtrace(void) {
	return fclose(tracefile);
	}
