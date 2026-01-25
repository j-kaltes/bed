
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#ifdef  __ANDROID__ 
#include <algorithm>
#include <iterator>
#endif
#include "system.h"
#include "main.h"
#ifdef HAVE_TERMIOS_H
       #include <termios.h>
#endif
       #include <unistd.h>
#ifndef NOSIGNALS
extern void stophandler(int signum) ;
#endif
#undef ERR
#include <signal.h>
#undef ERR
#ifdef GETOTHERTERM
extern int piddenlen;
extern pid_t *pidden;
#endif
#include "cursesinit.h"
#if defined(__MINGW32__)&&!defined(SIGTSTP)
#include "signalnums.h"
#endif

#ifndef HAVE_SETENV 
int setenv(const char *var,const char *value, int overwrite) {
	if(overwrite==1||!getenv(var)) {
		char buf[1024];
		snprintf(buf,1024,"%s=%s",var,value);
		return _putenv(buf);
		}
	return 0;
	}
int unsetenv(const char *var) {
	char buf[512];
	snprintf(buf,512,"%s=",var);
	return _putenv(buf);
	}
#endif
char * file2buf(const char *name,int *len) {
	char *buf;
	int end;
	int fd=open(name,O_RDONLY BINARY_FLAG);
	if(fd<0)
		return NULL;
	end=lseek(fd,0,SEEK_END);
	buf= (char *)malloc(end);	
	lseek(fd,0,SEEK_SET);
	if(read(fd,buf,end)<end) {
		free(buf);
		return NULL;
		}
	close(fd);
	*len=end;
	return buf;
	}
int readbuf(char *name,char *buf,int max) {
	int fd=open(name,O_RDONLY BINARY_FLAG);
	if(fd<0)
		return -1;
	int end=read(fd,buf,max);
	close(fd);
	return end;
	}
#include "shell.h"

#ifndef HAS_GETUSERSHELL
#ifndef  __ANDROID__ 
char **openshells(int *nr) {
	int len;
	char *shells= file2buf("/etc/shells",&len);
	if(!shells) {
		*nr=0;
		return NULL;
		}
	char *shelp;
	int i;
	char *endshell=shells+len;
	for(i=0,shelp=shells;shelp<endshell;i++) {
	       if(!(shelp=(char*)memchr(shelp, '\n', endshell-shelp))) {
			i++;
			break;
			}
		do {
			shelp++;
			} while((shelp[0]=='\n'));
		}
	char **shellen=(char **)malloc(i*sizeof(char *));
	int j;
	for(j=0,shelp=shells;j<i;j++) {
		shellen[j]=shelp;
	       if(!(shelp=(char*)memchr(shelp, '\n', endshell-shelp)))
			break;
		*shelp++='\0';
		 while((shelp[0]=='\n'))
			shelp++;
		}
	*nr=i;
	return shellen;
	}
#endif
#endif
#if defined(NOSTOPANDTHREADS) && defined(USETHREADS)
#define DONTSUSPEND 1
#endif
#ifdef PDCURSES
#define DONTSUSPEND 1
#endif
#if defined(PROCEXEFILENAMEFORMAT) && ! defined(DONTSUSPEND)
#define TRY_SUSPEND 1
#else
#undef TRY_SUSPEND 
#define DONT_TEST_SHELL

#endif
//#define  TRY_SUSPEND 
#define DONT_TEST_SHELL /*What use is it?*/
#ifndef DONT_TEST_SHELL
int isashell(pid_t pid) {
if(pid<(pid_t)0)
	return -1;
#ifndef HAS_GETUSERSHELL
#ifndef  __ANDROID__ 
	static int shellnr;
	static char **shells= openshells(&shellnr);
#else
	//const char * const shells[]={"/system/bin/mksh","/system/bin/sh","/system/xbin/bash","/system/xbin/ash","/system/xbin/dash","/data/data/com.termux/files/usr/bin/bash"};
	const char * const shells[]={"mksh","sh","bash","ash","dash"};
	const int shellnr=sizeof(shells)/sizeof(const char *);
#endif
#endif
	char cmdexe[256];
	snprintf(cmdexe,256,PROCEXEFILENAMEFORMAT,pid);
	char name[256];
	int len=readlink(cmdexe,name,256);
	name[len]='\0';
#ifndef HAS_GETUSERSHELL
#ifdef  __ANDROID__ 
auto endname=std::make_reverse_iterator(name);
const auto it=std::find( std::make_reverse_iterator(name+len),  endname,'/');
if(it==endname) {
	fprintf(stderr,"can't find / in %s\n",name);
	return -1;
	}
const char *nameptr=&*it+1;
#else
const char *nameptr=name;
#endif
	for(int i=0;i<shellnr;i++) {
		if(!strcmp(shells[i],nameptr))
			return 1;
		}
#else
	char *ashell=NULL;
	setusershell();
	while((ashell=getusershell())) {
		if(!strcmp(ashell,name)) {
			endusershell();
			return 1;
			}
		}
	endusershell();
#endif
	return 0;	
	}
#endif
char endmenustring[]={0,27,'[','?','1','0','l'};
char showmenustring[]={0,27,'[','?','1','0','h'};

extern const int endmenulen;
extern const int showmenulen;
const int endmenulen=sizeof(endmenustring);
const int showmenulen=sizeof(showmenustring);
extern int ismenu;
int startterminal=1;
extern void resetsize(void);
extern int	resizeconsole(int,int);

#ifdef HAVE_TERMIOS_H
struct termios  starttermios; 
int aget=tcgetattr (STDIN_FILENO,&starttermios );
#endif
#if defined(_WIN32)
extern CONSOLE_SCREEN_BUFFER_INFO original_screenbuffer;
#endif


#ifdef _WIN32
char *changeslash(char *in) {
	if(in) {
		int len=strlen(in);
		for(int i=0;i<len;i++)
			if(in[i]=='\\')
				in[i]='/';
		}
	return in;
	}
#endif

int startshell(void) {

SHOWCURSOR;
#ifdef NOENDWIN
wattrset(stdscr,0);
move(LINES-1,0);
addstr("\n");
refresh();
#else
if(!isendwin()) {
	endwin();
	}
#endif
#ifndef NOENDWIN
#ifdef HAVE_TERMIOS_H
if(!startterminal)  {
	tcsetattr(STDIN_FILENO, TCSANOW ,&starttermios );
	}
#endif
#endif
#ifdef __CYGWIN__x
static const char *SHELL=changeslash(getenv("SHELL"));
#else
static const char *SHELL=getenv("SHELL");
#endif
const char PS1[]="exit to return to bed\n";
/*
static const char *OLDPS1=(OLDPS1=getenv("PS1"))?OLDPS1:"\\w# ";
static int ps1len=(ps1len=strlen(PS1),
snprintf(PS1+ps1len,100-ps1len,"%s",OLDPS1), setenv("PS1",PS1,1), ps1len);
setenv("PS1",PS1,1);
*/

#ifdef USE_WITH_XBED
		char *oldenv=NULL;
		if(ismenu>=0) {
			oldenv=getenv("FROMXBED");
			unsetenv("FROMXBED");
			if(ismenu==1)
				write(STDOUT_FILENO,endmenustring,sizeof(endmenustring));
			}
#endif
		fputs(PS1,stdout);
if(SHELL) 
{
		process(SHELL);	
}
else {
#ifdef __ANDROID__
	const char shshell[]= "/system/bin/sh";
#else
	const char shshell[]= "/bin/sh";
#endif

	if(access(shshell,X_OK)==0) 
		process(shshell);	

#ifdef _WIN32
	else {
	static const char *prompt=(prompt=getenv("PROMPT"))?prompt:"$N$g";
	static char newprompt[100]="exit to return to bed$_";
	static int newpromptlen=(newpromptlen=strlen(newprompt),
	snprintf(newprompt+newpromptlen,100-newpromptlen,"%s",prompt), setenv("PROMPT",newprompt,1), newpromptlen);
	//static const char *comspec= changeslash( getenv("COMSPEC"));
	static const char *comspec= getenv("COMSPEC");
			if(comspec)
				process(comspec);	
			else return -1;
	}
#endif
}
	#ifdef USE_WITH_XBED
			if(ismenu>=0) {
				setenv("FROMXBED",oldenv,1);
				if(ismenu==1)
					write(STDOUT_FILENO,showmenustring,sizeof(showmenustring));
				}
	#endif
//setenv("PS1",OLDPS1,1);

return 0;

}
#ifndef DONTSUSPEND 
int realsuspend(void) {
#ifdef GETOTHERTERM
	if(piddenlen) {
		move(thelines-2,0);
		clrtoeol();
		printw("kill -18 %d to return to bed",pid);
		move(thelines-1,0);
		clrtoeol();
		touchline(stdscr,thelines-2,2);
		refresh();
		for(int i=(piddenlen-1);i>=0;--i)
			if(pidden[i]!=pid)
				kill(pidden[i],SIGTSTP);
		for(int i=(piddenlen-1);i>=0;--i)
			if(pidden[i]!=pid)
				kill(pidden[i],SIGWINCH);
		}
#endif
extern void (*origstophandler)(int);
	if(origstophandler&&origstophandler!=SIG_IGN) {
#ifndef NOENDWIN
if(!isendwin()) {
 	SHOWCURSOR;
	endwin();
	}
#endif
		origstophandler(SIGSTOP);
		return 0;
		}
	return -1;
//	return kill( getpgrp(),SIGSTOP);
}
#endif 
int suspender(bool susp) {

#ifndef DONTSUSPEND 
#ifdef TRY_SUSPEND
#ifndef DONT_TEST_SHELL
 if(susp) {
	pid_t par=getppid();
	 pid_t sid=getsid(pid);
		susp =(isashell(par)||isashell(sid));
 }
#endif
#endif

	if(startterminal&&susp) {
if(realsuspend()==0)
	return 0;

}
#endif
{
int res=startshell();
#ifndef NOSIGNALS
#ifndef PDCURSES
extern void conthandler(int signum) ;
conthandler(SIGCONT);
#endif
#endif
return res;

		}

	}
extern int nasuspend(void) ;
int nasuspend(void) {
//fprintf(stderr,"after suspend\n");
#if  defined(_WIN32)
#ifdef NCURSES_VERSION
	if(consoleoutputhandle!=INVALID_HANDLE_VALUE) 
#endif
     GetConsoleScreenBufferInfo(consoleoutputhandle, &original_screenbuffer);
#endif
#ifndef NOENDWIN
if(isendwin())
	refresh();
#endif
	HIDECURSOR;
    resizeflag=1;

#if  defined(_WIN32)
#ifdef NCURSES_VERSION
	if(consoleoutputhandle!=INVALID_HANDLE_VALUE)  {
	resized();
	rewriteall();
	}
#endif
resized();
#else
if(curses_init_curses.isresized()) { 
        resized(); 
        rewriteall(); 
        }

#endif
return 0;
}

int thesuspend(bool susp=true) {

#if  defined(_WIN32)
#if defined(NCURSES_VERSION) 
if(consoleoutputhandle!=INVALID_HANDLE_VALUE) 
#endif
{
	SetConsoleScreenBufferSize(consoleoutputhandle,original_screenbuffer.dwSize);
		}	
#endif
int res=	 suspender(susp) ;

#ifdef PDCURSES // Kan waarschijnlijk na startshell();
nasuspend() ;
#endif
return res;
	}

