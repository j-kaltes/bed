
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

       #include <sys/types.h>
       #include <sys/wait.h>
#include "varmacro.h"
#ifdef __CYGWIN__
#include <sys/errno.h>
#define sys_errlist _sys_errlist
#define sys_nerr _sys_nerr
#endif


#define DEBUG(x) 

static pid_t filepid=0;
#define READ 0
#define WRITE 1
static int fromchild[2]={0,0};
  #include <string.h>
//static struct sigaction oldsact,newsact;
void (*oldpipehandler)(int)=SIG_DFL;
static void pipehandler(int signr) {
	if(filepid) {
		close(fromchild[READ]);
		kill(filepid,9);
		}
	filepid=0;
//	signal(signr,oldsact.sa_handler);
	signal(signr,oldpipehandler);
	}
#endif



#ifdef EDITOR





#ifdef OLDGCCVARMACRO
#define fprintf(x,args...) {message(##args);erefresh();}
#else
#define fprintf(x,...) {message(__VA_ARGS__);erefresh();}
#endif


     /* Read characters from the pipe and echo them to `stdout'. */
//#define DEBUG(x) write(STDERR_FILENO,x,strlen(x));
int filetype (void);





#endif

call(filetype)

#ifdef UNDER

extern int mytmpname(char *buf,int len,const char *name) ;
int Editor::filetype (void) {
int datahandle;
char datafile[256];
if(mytmpname(datafile,256,"bed-file-XXXXXX")<0) {
	message("filetype: Failed to create tmpfile");
		return -2;
	}
datahandle=creat(datafile,0600);
if(datahandle<0) {
		message("filetype: Failed to create tmpfile");
		return -1;
		}
		 int len=4096
#if defined (__sun) || defined (sun)
*1024
#endif
,size;
//         int len=4096*1024,size;
		 char buf[len];
	size=getmem(geteditpos()+getfilepos(),buf, len);
	ssize_t did=0,ret;
	while(did<size) {
		ret=write(datahandle,buf,size-did);
		if(ret<0) {
			message("error writing to file");
			close(datahandle);
			return -1;
			}
		did+=ret;
		}
	close(datahandle);
	oldpipehandler=signal(SIGPIPE,pipehandler);
		if(pipe(fromchild)==-1) {
			message("filetype: pipe failed");
			return -1;
			}
	 filepid = fork ();
       if (filepid == (pid_t) 0)
         {
				close(fromchild[READ]);
          close(STDOUT_FILENO);
          if(dup(fromchild[WRITE])<0) {
         	fprintf(stderr,"dup2(fromchild[WRITE],STDOUT_FILENO)<0 failt: %s", strerror(errno) );
         	return -1;
          	}
//         if(execlp("file","file","-z","-b", datafile,NULL)<0) {
// using -z blocks using file-4.02 as supplied by fedora 1
         if(execlp("file","file",
#ifdef UNCOMPRESS_FILE
"-z", 
#endif
//#if defined(__linux__)||defined(__CYGWIN__)
#ifdef BRIEF_FILE
"-b", 
#endif
datafile,NULL)<0) {
		message("while trying to execute file: %s",strerror(errno));
		erefresh();
         	exit(2);
           	}
           return -1;
         }
       else if (filepid < (pid_t) 0)
         {
           /* The fork failed. */
           fprintf (stderr, "Fork failed.");
           return -1;
         }
       else
         {
	
	int status;
	 close(fromchild[WRITE]);
	waitpid(filepid,&status,0);
       if(!WIFEXITED(status)) {
		message("Didn't exit normally");erefresh();
		}
//       pid_t waitpid(pid_t pid, int *status, int options);
	size=read(fromchild[READ],buf,len);
	if(size<0) {
		message("Error reading from pipe");erefresh();
		}
else {
	if(size>0) {
			buf[size-1]='\0';
		//	message(buf+29);
#ifdef BRIEF_FILE
			message(buf);
#else
			message(buf+strlen(datafile)+2);
#endif
			}
		}

 remove(datafile);
	close(fromchild[READ]);
	return 0;
         }
      return -1;
     }

sethelp(filetype,"Uses the unix program file to guess the filetype of the data starting at\nthe current cursor position. It can be used when the cursor is on the\nfirst byte of a file or when searching for files inside a harddrive device.");
#endif
