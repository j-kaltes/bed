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
static int tochild[2]={0,0};
static int fromchild[2]={0,0};
  #include <string.h>
//static struct sigaction oldsact,newsact;
void (*oldpipehandler)(int)=SIG_DFL;
static void pipehandler(int signr) {
	if(filepid) {
		close(tochild[WRITE]);
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
int Editor::filetype (void) {
/*
	newsact.sa_handler=pipehandler;
	sigemptyset(&newsact.sa_mask);
	newsact.sa_flags=SA_RESTART;
	sigaction(SIGPIPE,&newsact,&oldsact);
	*/
	oldpipehandler=signal(SIGPIPE,pipehandler);
     	if(pipe(tochild)==-1) 
     		return -1;
     	if(pipe(fromchild)==-1) 
     		return -1;
       filepid = fork ();
       if (filepid == (pid_t) 0)
         {
	close(tochild[WRITE]);
	close(fromchild[READ]);
          close(STDERR_FILENO);
         close(STDIN_FILENO);
         if(  dup(tochild[READ])<0) {

         	fprintf(stderr,"dup2(tochild[READ],STDIN_FILENO)<0 failt: %s", strerror(errno) );
         	return -1;
          	}
          close(STDOUT_FILENO);
          if(dup(fromchild[WRITE])<0) {
         	fprintf(stderr,"dup2(fromchild[WRITE],STDOUT_FILENO)<0 failt: %s", strerror(errno) );
         	return -1;
          	}
         if(execlp("file","file","-b","-",NULL)<0) {
         	fprintf(stderr,"execlp: %s", strerror(errno) );
         	return -1;
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
         int len=4096,size;
//         int len=4096*1024,size;
         char buf[len];
	 close(tochild[READ]);
	 close(fromchild[WRITE]);
	size=getmem(geteditpos()+getfilepos(),buf, len);
	buf[size-1]=EOF;
	buf[size-2]='\0';
	buf[size-3]='\n';
	ssize_t did=0,ret;
	while(did<size) {
		ret=write(tochild[WRITE],buf,size-did);
		if(ret<0) {
			message("error writing to pipe");
		if(filepid)
			kill(filepid,9);
		close(tochild[WRITE]);
		close(fromchild[READ]);
		return -1;
			}
		did+=ret;
		}
	int status;
		message("file.plug: write");erefresh();
	waitpid(filepid,&status,0);
       if(!WIFEXITED(status)) {
		message("Didn't exit normally");erefresh();
		}
//       pid_t waitpid(pid_t pid, int *status, int options);
		message("file.plug: before read");erefresh();
	size=read(fromchild[READ],buf,len);
	if(size<0) {
		message("Error reading from pipe");erefresh();
		}
else {
	if(size>0) {
			buf[size-1]='\0';
		//	message(buf+29);
			message(buf);
			}
		}

//	kill(filepid,9);
	close(tochild[WRITE]);
	close(fromchild[READ]);
	return 0;
         }
      return -1;
     }

sethelp(filetype,"Uses the unix program file to guess the filetype of the data starting at\nthe current cursor position. It can be used when the cursor is on the\nfirst byte of a file or when searching for files inside a harddrive device.");
#endif
