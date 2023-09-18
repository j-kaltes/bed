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
#include "global.h"
BEDPLUGIN
#include <errno.h>
#include <strings.h>
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
#include "varmacro.h"

extern Editor	*newfile(const char *);



static pid_t gdbpid=0;
#define READ 0
#define WRITE 1
static int tochild[2]={0,0};
static int fromchild[2]={0,0};
#ifdef STATIC
char lookfile[]= CONFDIRLINK "/disass-use";
#else
char lookfile[]=EXAMPLEDIR "/disass-use";
#endif
   #include <string.h>
//static struct sigaction oldsact,newsact;
void (*oldsighandler)(int)=SIG_DFL;
static void pipehandler(int signr) {
	if(gdbpid) {
		close(tochild[WRITE]);
		close(fromchild[READ]);
		kill(gdbpid,9);
		}
	gdbpid=0;
	signal(signr,oldsighandler);
	}







#ifdef OLDGCCVARMACRO
#define	debugmessage(mes...) 
#define wasfprintf(x,mes...) {message(mes);erefresh();}
#else
#define wasfprintf(x,...) {message(__VA_ARGS__);erefresh();}

#define	debugmessage(...) 
//#define debugmessage(...) wasfprintf(stderr,__VA_ARGS__)
#endif


     /* Read characters from the pipe and echo them to `stdout'. */
//#define DEBUG(x) write(STDERR_FILENO,x,strlen(x));
//

#define DEBUG(...) 
/*FILE *outputerrors=fopen("/tmp/gdberrors","a");
#define DEBUG(...) (fprintf(outputerrors,__VA_ARGS__),fflush(outputerrors));
*/
static OFFTYPE waspos;
static int waslen;
class Editor {

public:
#include "faked.h"
int gdbstartup (void) {
	waspos=-1;
	waslen=-1;
	/*
	newsact.sa_handler=pipehandler;
	sigemptyset(&newsact.sa_mask);
	newsact.sa_flags=SA_RESTART;
	sigaction(SIGPIPE,&newsact,&oldsact);
	*/
	oldsighandler=signal(SIGPIPE,pipehandler);
     	if(pipe(tochild)==-1) 
     		return -1;
     	if(pipe(fromchild)==-1) 
     		return -1;
     	message("Starting gdb. Wait ...");
     	erefresh();
       gdbpid = fork ();
       if (gdbpid == (pid_t) 0)
         {
	close(tochild[WRITE]);
	close(fromchild[READ]);
         close(STDIN_FILENO);
         if(  dup(tochild[READ])<0) {
//s/sys_errlist\[errno\]/strerror(errno)/g
         	wasfprintf(stderr,"dup2(tochild[READ],STDIN_FILENO)<0 failt: %s", strerror(errno) );
         	return -1;
          	}
          close(STDOUT_FILENO);
          if(dup(fromchild[WRITE])<0) {
         	wasfprintf(stderr,"dup2(fromchild[WRITE],STDOUT_FILENO)<0 failt: %s", strerror(errno) );
         	return -1;
          	}
#ifdef __CYGWIN__
         if(execlp("gdb","gdb",lookfile, "--nw","-q",NULL)<0) {
#else
         if(execlp("gdb","gdb",lookfile, "-q",NULL)<0) {
#endif
         	wasfprintf(stderr,"execlp(gdb...): %s", strerror(errno) );
		message("starting gdb: %s",strerror(errno) );erefresh();
         	exit( -1);
           	}
           return -1;
         }
       else if (gdbpid < (pid_t) 0)
         {
           /* The fork failed. */
           wasfprintf (stderr, "Fork failed.");
           return -1;
         }
       else
         {
#warning "Change sleep(1) to something better"
//         int size,bufsize=1024;
         int size,bufsize=4096;
         char buf[bufsize],*ptr,*newptr;
	 char breaks[]="break main\n";
	 char run[]="run\n",ch;
	 close(tochild[READ]);
	 close(fromchild[WRITE]);
waitongdb:
	size=read(fromchild[READ],buf,bufsize);
	DEBUG("read1=%d ",size)
	while(size==0) {
		write(tochild[WRITE],run,strlen(run));
		size=read(fromchild[READ],buf,bufsize);
	DEBUG("read2=%d ",size)
		}; 
	if(size<0){
		wasfprintf(stderr,"gdb doesn't answer");
		return -1;
		}
	debugmessage("Processing answer");erefresh();
	while(size<5)
		size+=read(fromchild[READ],buf+size,bufsize-size);
	ch=buf[size];
	buf[size]=0;
	if(!(ptr=strstr(buf,"(gdb)"))) {
		DEBUG("No (gdb) ");
		static int time=0;
		if(time++<1) {
			sleep(1);
			goto waitongdb;
			}
		message("gdb doesn't start with (gdb)");
		return -1;
		}
		DEBUG("(gdb) ");
	buf[size]=ch;
	ptr+=5;
	debugmessage("Found (gdb)");erefresh();
	write(tochild[WRITE],breaks,strlen(breaks));
	debugmessage("break main");
     	erefresh();
	size+=read(fromchild[READ],buf+size,bufsize-size);
	if((buf+size)==ptr) {
		message("failt reading from gdb pid=%d",gdbpid);
		return -1;
		}
	ch=buf[size];
	buf[size]='\0';
	if((newptr=strstr(ptr,breaks))) {
		debugmessage("Found break setting 1\n");erefresh();
		ptr=newptr+11;
		}
//	buf[size]=ch;
	while((buf+size-ptr)<10) {
		if((newptr=strstr(ptr,"(gdb)"))) {
			message("Error: %s should be compiled with -g",lookfile);
			return -1;
			}
		debugmessage("Read %d after:%s:\n",buf+size-ptr,ptr); ;erefresh();
		buf[size]=ch;
		size+=read(fromchild[READ],buf+size,bufsize-size);
		ch=buf[size];
		buf[size]='\0';
		if((newptr=strstr(ptr,"break mai"))) {
			debugmessage("Found break setting 2\n"); ;erefresh();
			while((buf+size-newptr)<(int)sizeof(breaks)) 
				size+=read(fromchild[READ],buf+size,bufsize-size);
			ptr=newptr+(sizeof(breaks)-1);
			}
		}
//	buf[size]=ch;

//	size+=read(fromchild[READ],buf+size,bufsize-size);
	buf[size]='\0';
	
	debugmessage("Before Breakpoint with:%s:",ptr); ;erefresh();
	if((newptr=strstr(ptr,"Function"))) {
		debugmessage("Recompile with -g: %s",ptr);
		erefresh();
		return -1;
		}
	newptr=ptr;
	if(!(ptr=strstr(ptr,"Breakpoint"))) {
	//	size+=read(fromchild[READ],buf+size,bufsize-size);
	//	buf[size]='\0';
		message("Failt setting breakpoint: %s",newptr);erefresh();
		return -1;
		}
	buf[size]=ch;
	ptr+=10;
	while(!strstr(ptr,"(gdb)")) {
		size+=read(fromchild[READ],buf+size,bufsize-size);
		buf[size]='\0';
		}
	write(tochild[WRITE],run,strlen(run));
	size=read(fromchild[READ],buf,bufsize);
	if(size<0) {
		message("failt reading from gdb pid=%d",gdbpid);
		return -1;
		}
	while(!strstr(buf,"Starting")){
		size+=read(fromchild[READ],buf+size,bufsize-size);
		buf[size]='\0';
	//	wasfprintf(stderr,"Cant execute help program");
	//	return -1;
		}
	buf[size]='\0';
	while(!strstr(buf,"(gdb)")) {
		size+=read(fromchild[READ],buf+size,bufsize-size);
		buf[size]='\0';
		}
	DEBUG("END Startup")
	return 0;
         }
      return -1;
     }
int gdbsane(void) {
//	int bufsize=1024,size;
	int bufsize=4096*10,size;
	char out[]="ptype cursor\n";
	char in[]="type = unsigned long",*res;
	char buf[bufsize];
         if(!gdbpid) {
			if(gdbstartup()<0)  {
				close(tochild[WRITE]);
				close(fromchild[READ]);
				if(gdbpid) {
					kill(gdbpid,9);
					}
				gdbpid=0;
				return -1;
				}
         	}

//fdatasync(fromchild[READ]);
        lseek(fromchild[READ],0L,SEEK_END);
//fdatasync(fromchild[READ]);
/*
	while(read(fromchild[READ],buf,bufsize)>0)
		;
*/
	write(tochild[WRITE],out,strlen(out));
	size=read(fromchild[READ],buf,bufsize);
	if(size<0) {
		message("gdbsane: failt reading from gdb pid=%d. Try again",gdbpid);
		return -1;
		}
	buf[size]='\0';
	while(!strstr(buf,"(gdb)")) {
		size+=read(fromchild[READ],buf+size,bufsize-size);
		buf[size]='\0';
		}

	while(!(res= strstr(buf,in))){
		size+=read(fromchild[READ],buf+size,bufsize-size);
		buf[size]='\0';
		}
/*
	res= strstr(buf,in);

	if(!res) {
		message("%s instead of \'type = long\'",buf);
		return -1;
		}
*/
	return 0;
	}

int bin2ass(char *inp,char *output,int max) {
const int bufsize=1024*4;
         char buf[bufsize+1],*ptr,*start,*end;
         if(gdbsane()!=0) {
         	return -1;
         	}
	unsigned int *gegs=(unsigned int *)inp;

	sprintf( buf,"p cursor[0]=0x%x\np cursor[1]=0x%x\np cursor[2]=0x%x\np cursor[3]=0x%x\np cursor[4]=0x%x\ndisassemble cursor,cursor+4\n",gegs[0],gegs[1],gegs[2],gegs[3],gegs[4]);
	write(tochild[WRITE],buf,strlen(buf));
	ptr=NULL;
	DEBUG("before read")
	start=buf;
	int size=0;
	while(size<bufsize&&((size+=read(fromchild[READ],start,bufsize-size))>=0)) {
DEBUG("read3=%d ",size);
//		write(STDOUT_FILENO,buf,size);
		start=buf+size;
		start[0]='\0';
		DEBUG(buf)
		if((ptr=strstr(buf,"End of assembler dump.")))
			break;
		}
DEBUG("after while");
	if(ptr) {
DEBUG("ptr=%s",ptr);
		int len=0xffff;

		if(!(start=strstr(buf,"Dump of assembler")))
//		if(!(start=strstr(buf,"cursor:")))
			return -1;
#ifndef index
#define index strchr
#endif
		if(!(start=index(start,':')))
			return -1;
		start+=2;
		if(!(end=index(start,'>')))
			return -1;
		if(!(ptr=index(end,'\n')))
			return -1;
		*ptr++='\0';
		if((ptr=index(ptr,'+'))) 
			sscanf(++ptr,"%d",&len);
		if((end=index(end,'<'))&& (ptr=index(end,'>'))) {
			*end='\0';
			snprintf(output,max, "%s%s",start,++ptr);
			}
		else	
			snprintf(output,max,"%s",start);
		return len;
		}
#undef index	
	DEBUG("end gdbagain")
	return 0;
	}
int gdbagain(char *inp) {
const int max=80;
	char output[max+1];
	int len= bin2ass(inp,output,max); 
	if(len>0) {
		message("%s		(%d bytes)",output,len);
		waslen=len;
		}
	else {
		waspos=-1;
		return -1;
		}
	return len;
	}
#define TAKE 25

int disassembler(void) {
	char buf[TAKE];
	memset(buf,'\0',TAKE);
	waspos=getfilepos()+geteditpos();
	getmem(waspos,  buf,TAKE); 
	return gdbagain(buf);
	};

int nextdisassem(void) {
	const int max=1;
	char buf[TAKE];
	char output[max+1];
	OFFTYPE pos=getfilepos()+geteditpos();
	if(waspos!=pos) {
		getmem(pos,  buf,TAKE); 
		waslen= bin2ass(buf,output,max); 
		if(waslen<=0) {
			waspos=-1;
			message("conversion failt");
			return -1;
			}
		waspos=pos;
		}
	toborderpos(pos+waslen);
	return disassembler();
	}
};
sethelp(disassembler,"File content at cursor position is translated to assembler code.\nMakes use of gdb. Use nextdisassem to jump to the next machine code.\nUse gotosymol (elftables.plug) to jump to procedure using its\nprocedure name" )
sethelp(nextdisassem,"Jumps to next machine code instruction and disassembles it:\nThe cursor position is moved the length of the current machine code\nstatement further and the disassembler is called." )
call(disassembler)
call(nextdisassem)
