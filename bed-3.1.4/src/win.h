#ifndef WIN_H
#define WIN_H
#include "offtypes.h"
#if  defined(_WIN32)||defined(_WIN64)
#include "winpad.h"
#include <windows.h>
#endif

#ifdef __CYGWIN__
#include <sys/cygwin.h>
//#include "system.h"
#include <cygwin/version.h>
#include "offtypes.h"
#if CYGWIN_VERSION_DLL_MAJOR<1007
inline ssize_t		abswinpath(const char *in,char *out) {
	return cygwin_conv_to_full_win32_path(in,out);
	}
inline ssize_t		winpath(const char *in,char *out) {
	return cygwin_conv_to_win32_path(in,out);
	}
inline ssize_t		absunixpath(const char *in,char *out) {
	return cygwin_conv_to_full_posix_path(in,out);
	}
#else
inline ssize_t		abswinpath(const char *in,char *out) {
	return cygwin_conv_path (CCP_POSIX_TO_WIN_A,in ,out, MAX_PATH);
	}

inline ssize_t		winpath(const char *in,char *out) {
	return cygwin_conv_path (CCP_POSIX_TO_WIN_A| CCP_RELATIVE,in ,out, MAX_PATH);
	}

inline ssize_t		absunixpath(const char *in,char *out) {
	return cygwin_conv_path(CCP_WIN_A_TO_POSIX,in,out, MAX_PATH);
	}
#endif
#endif
#ifdef _WIN32
inline FILEHANDLE opencreatenew(const char *filename) {
return CreateFile(
    filename,	// address of name of the file 
    GENERIC_WRITE,	// access (read-write) mode 
    FILE_SHARE_READ|FILE_SHARE_WRITE,	// share mode 
    NULL,	// address of security descriptor 
    CREATE_NEW,	// how to create 
    FILE_ATTRIBUTE_NORMAL,	// file attributes 
    NULL 	// handle of file with attributes to copy  
   );
	}

extern int globaloutput(char *str); 
extern int globalmessage(const char *format, ...) ;
extern char messagebuf[];
extern const int maxmessagebuf;

#define WINERROR 1234
inline DWORD winerror(char *mes=messagebuf,int maxmes=maxmessagebuf,DWORD dw = GetLastError()) {
		if(mes!= messagebuf)
			mes[0]='\0';
	if(dw) {
	     return FormatMessageA(
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_MAX_WIDTH_MASK,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		mes,
		maxmes, NULL );
		}
	else {
		return 0;
		}
	};

extern int thelines;
inline void errorline(void) {
	const int meslen=200;
	char mes[meslen];
	if(winerror(mes,meslen))
		globaloutput(mes);
	}
//inline void winerrormessage(char *form) {
#define winerrormessage(...) {\
	const int meslen=200;\
	char mes[meslen];\
	if(winerror(mes,meslen)) {\
		globalmessage(__VA_ARGS__,mes);\
		refresh();\
		}\
	}
#define showerror(...) {\
	const int meslen=200;\
	char mes[meslen];\
	if(winerror(mes,meslen)) {\
		message(__VA_ARGS__, mes);\
		refresh();\
		}\
	}

inline bool samelink( BY_HANDLE_FILE_INFORMATION *one, BY_HANDLE_FILE_INFORMATION *two) {

#define gelijk(info) (one->info==two->info)
/*#define gelijklow(info) ((0xFFFF&one->info)==(0xFFFF&two->info))
#define gelijkhigh(info) ((0xFFFF0000&one->info)==(0xFFFF0000&two->info))
*/
return (one->nNumberOfLinks&& two->nNumberOfLinks&& gelijk(dwVolumeSerialNumber)&& gelijk(nFileIndexLow)&&gelijk(nFileIndexHigh));
#undef gelijk
}
#endif
#endif
