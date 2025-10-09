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
#ifdef DEBUGON
#include <features.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
FILE *debugfile;
int debugging=1;
extern int pid;

#define DEBUGSIZE 256

int startdebug(void) {
	time_t tim;
	char debugname[DEBUGSIZE];
	char *user;
	struct passwd *pas;
	if((pas=getpwuid(getuid())))
		user=pas->pw_name;
	else 
		user="";
	snprintf(debugname,DEBUGSIZE,"/tmp/beddebug_%s.%d",user,pid);
	if(!debugging)
		return -1;
	debugfile=fopen(debugname, "a");
	setvbuf(debugfile,NULL,  _IONBF,0);
	time(&tim);
	fprintf(debugfile,"\nRestart %d at %s",pid,ctime(&tim));
	return 0;
	}
#endif
