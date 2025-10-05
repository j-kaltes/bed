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
#include <magic.h>

#endif



#ifdef EDITOR






     /* Read characters from the pipe and echo them to `stdout'. */
//#define DEBUG(x) write(STDERR_FILENO,x,strlen(x));
int filetype (void);





#endif

call(filetype)

#ifdef UNDER

int Editor::filetype (void) {
	 OFFTYPE len=4096
#if defined (__sun) || defined (sun)
*1024
#endif
, size;
 char buf[len];
magic_t ma= magic_open( MAGIC_SYMLINK  | MAGIC_COMPRESS );
 if(!ma) {
	message("magic_open failed");
	return 1;
	}
 if(magic_load(ma,NULL)<0) {
	message("magic_load: %s",magic_error(ma));
	magic_close(ma);
	return 2;
	}


size=getmem(geteditpos()+getfilepos(),buf, len);
if(size<=0) {
	magic_close(ma);
	message("read failure ");
	return 3;
	}
 const char * const desc=magic_buffer(ma, buf,len);
 if(!desc) {
	message("magic_buffer: %s",magic_error(ma));
	magic_close(ma);
	return 4;
	}
message(desc);
	magic_close(ma);
      return 0;
     }

sethelp(filetype,"Uses libmagic to guess the filetype of the data starting at\nthe current cursor position. It can be used when the cursor is on the\nfirst byte of a file or when searching for files inside a hard drive device.");
#endif
