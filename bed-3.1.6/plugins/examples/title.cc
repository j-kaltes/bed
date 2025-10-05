#include <sys/unistd.h>
#include "global.h" 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "config.h"
#include CURSES_HEADER
extern char *  ttyname (int __fildes);
#ifdef _WIN32
#ifdef PDCURSES
#undef MOUSE_MOVED
#endif
#include <windows.h>
#endif
BEDPLUGIN
extern int withinxterm;
#if !defined(PDCURSES)
static const char titlestart[]="\033]0;";
static const char titleend[]="\007";
#define setesctitle(titlebuf) if(withinxterm)  {write(STDOUT_FILENO,titlestart,sizeof(titlestart)-1);write(STDOUT_FILENO ,titlebuf,strlen(titlebuf));write(STDOUT_FILENO ,titleend,sizeof(titleend)-1);}
#endif
#ifdef _WIN32
#ifdef PDCURSES
//#define settitle(titlebuf) SetConsoleTitle(TEXT(titlebuf))
#define settitle(titlebuf) SetConsoleTitle(titlebuf)
#else
void settitle(const char *titlebuf) {
//	SetConsoleTitle(TEXT(titlebuf));
	SetConsoleTitle(titlebuf);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	setesctitle(titlebuf);
#pragma GCC diagnostic pop
}
#endif
#else
void settitle(const char *titlebuf) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	setesctitle(titlebuf)
#pragma GCC diagnostic pop
    }
#endif
void endtitle(void) {
	settitle("Unknown");
	}
#ifndef PROGRAM
#define PROGRAM "bed"
#endif
#ifndef PDCURSES
#define TTYNAME
#endif
#ifndef  _WIN32
#define HOSTNAME
#endif
/*
#ifdef HAS_TTYNAME_R
__BEGIN_DECLS
extern int ttyname_r (int __fd, char *__buf, size_t __buflen);
__END_DECLS
#endif
*/

#ifdef  _WIN32
//Adaption of the example in http://msdn.microsoft.com/en-us/library/aa376389%28VS.85%29.aspx
BOOL getadmin(void) {
BOOL b;
SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
PSID AdministratorsGroup; 
b = AllocateAndInitializeSid(
    &NtAuthority,
    2,
    SECURITY_BUILTIN_DOMAIN_RID,
    DOMAIN_ALIAS_RID_ADMINS,
    0, 0, 0, 0, 0, 0,
    &AdministratorsGroup); 
if(b) 
{
typedef BOOL WINAPI (*CheckTokenMembershipType)( HANDLE TokenHandle, PSID SidToCheck,PBOOL IsMember);
if(HMODULE advapi32=GetModuleHandle( "Advapi32.dll")) 
  if(CheckTokenMembershipType CheckTokenMembership =(CheckTokenMembershipType) GetProcAddress(advapi32,"CheckTokenMembership")) 
    if (!CheckTokenMembership( NULL, AdministratorsGroup, &b)) 
         b = FALSE;
FreeSid(AdministratorsGroup); 
}
return b;
}
BOOL isadmin=getadmin();
	
#endif //  _WIN32
#if defined(DEBUGFLAG) || defined(__ANDROID__)
extern pid_t pid;
#endif
class Editor {
public:
int filetype(void);
int addfile(void);
char *getfilename(char *buf);
int titleset(void) {
	const int MAXTITLE=200;
	static const char keep_unused_variable __attribute__ ((unused)) =(atexit(endtitle),1);
/*
#ifdef HOSTNAME
	static char hostname[50];
	static char *ptr=(((hostname[0]='\0',gethostname(hostname, 50),(ptr=strchr(hostname,'.'))?*ptr='\0':1)),ptr);
	static int hostnamelen=strlen(hostname);
#endif
*/
	const int proglen=sizeof(PROGRAM);
	char titlebuf[MAXTITLE+1];
	int nr=0;
#ifdef  _WIN32
if(isadmin) {
	strcpy(titlebuf,"Adm: ");
	nr+=5;
	}
#endif
#ifdef DEBUGFLAG
	nr+=sprintf(titlebuf+nr,"%d ",pid);
#endif

	memcpy(titlebuf+nr,PROGRAM,proglen-1);

	nr+=proglen;
	titlebuf[nr-1]=' ';
	
#ifdef HOSTNAME
	if(MAXTITLE>nr) {
class Host {
char hostname[10];
int len;
public:
inline Host() {
	gethostname(hostname, 10);
	char *ptr
#ifdef HAVE_STRCHRNUL
	=strchrnul(hostname,'.');
	{
#else
	=strchr(hostname,'.');
	if(!ptr) {
		len=strlen(hostname);
		}
	else {
#endif
	len=ptr-hostname;
	*ptr='\0';
	}
	};
inline char *name()  {return hostname;};
inline int size()  {return len;};
};
		static Host host;
#ifdef HAVE_MEMCCPY
		memccpy(titlebuf+nr,host.name(),'\0',MAXTITLE-nr);
#else
		strncpy(titlebuf+nr,host.name(),MAXTITLE-nr);
#endif
		nr+=host.size();
		titlebuf[nr++]='-';
		}
#endif
#ifdef TTYNAME
#ifdef  _WIN32
extern int dosconsole(void) ;
if(!dosconsole()) 
#endif
{
#if defined(HAS_TTYNAME_R)|| defined( __ANDROID__)
	char nametty[15];
#else
	char *nametty;
#endif	
//	ptsname_r(titlebuf,ttys[i],MAXTITLE-nr);
	if(isatty(STDOUT_FILENO)) {
#ifdef __ANDROID__
withinxterm=1;
		const int pmax=23;
		char procname[pmax];
		snprintf(procname,pmax,"/proc/%d/fd/0",pid);
		int len; 
		if((len=readlink(procname, nametty, 15))>0&&len<15) {
			nametty[len]='\0';
#else
#ifdef HAS_TTYNAME_R
		if(!ttyname_r(STDOUT_FILENO,nametty,15)) {
#else
		if((nametty=ttyname(STDOUT_FILENO))) {
#endif
#endif
	if(MAXTITLE>nr) {
#ifdef HAVE_MEMCCPY
			memccpy(titlebuf+nr,nametty+5,'\0',MAXTITLE-nr);
#else
		strncpy(titlebuf+nr,nametty+5,MAXTITLE-nr);
#endif

			nr+=strlen(titlebuf+nr);
			}
			}
		}
	if(nr<MAXTITLE)
		titlebuf[nr++]=' ';

}
#endif
	if(MAXTITLE>nr) {

#ifdef HAVE_MEMCCPY
		memccpy(titlebuf+nr,getfilename(NULL),'\0',MAXTITLE-nr);
#else
		strncpy(titlebuf+nr,getfilename(NULL),MAXTITLE-nr);
#endif
		}
	titlebuf[MAXTITLE]='\0';
	settitle(titlebuf);
	return 0;
	};
};




call(titleset)
