#include "global.h" 
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif
BEDPLUGIN

class Editor {
public:
#include "faked.h"
int insertshown(char *gegs, off_t endf) ;

iscalled(pastclipboard)
iscalled(toclipboard)
};

int Editor::pastclipboard(void) {
#ifdef _WIN32
int ret;
 HANDLE handle;
 long len=0;
 if (OpenClipboard(NULL) == 0)
 {
	message("can't openclipboard");
    return -1;
 }
 handle = GetClipboardData( CF_TEXT );
 if (handle == NULL)
 {
	message("can't get clipboarddata");
    CloseClipboard();
    return -1;
 }

 len = strlen((char *)handle);
 ret=insertshown((char *)handle, len); 
 CloseClipboard();
if(ret>=0)
	message("Pasted %d bytes from clipboard",len);
return ret ;
#else
#ifdef PDCURSES
char *cont;
long *len;
if(PDC_getclipboard(cont, len)!= PDC_CLIP_SUCCESS ) {
	message("Can't get clipboard");
	return -1;
	}
 ret=insertshown(cont, len); 
free(cont);
if(ret>=0)
	message("Retrieved %d bytes from clipboard",len);
return ret;
#else
message("Not implemented");
return -1 ;
#endif
#endif
}

extern int mytmpname(char *buf,int len,const char *name) ;
int Editor::toclipboard(void) {
#ifdef _WIN32
char tmpname[512];
int filelen;
 HGLOBAL ptr1;
 LPTSTR ptr2;
 if (OpenClipboard(NULL) == 0)
 {
	message("Can't open clipboard");
    return -1;
 }
if((mytmpname(tmpname,512,"bedtmpXXXXXX"))<0 ) {
	message("temporary file failed");return -1;
	}
if((filelen=saveshownname(tmpname))<0) {
	if(filelen==-1)
		message("No selection");
	unlink(tmpname);
	return -1;
	}
 ptr1 = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE, (filelen+1)*sizeof(TCHAR) );
 if (!ptr1)
 {
	unlink(tmpname);
	message("GlobalAlloc failed");
    return -1;
 }

 ptr2 =(LPTSTR) GlobalLock(ptr1);
int handle=open(tmpname,O_RDONLY|O_BINARY);
if(handle<0) {
	unlink(tmpname);
	 GlobalUnlock(ptr1);
      GlobalFree(ptr1);
	message("canot open %s",tmpname);
	return -1;
	}
int did=read(handle,ptr2,filelen);
close(handle);
unlink(tmpname);
if(did!=filelen) {
	 GlobalUnlock(ptr1);
      GlobalFree(ptr1);
	message("read not enough %s",tmpname);
	return -1;
	}
ptr2[filelen]='\0';
 GlobalUnlock(ptr1);
 EmptyClipboard();
 if (SetClipboardData(CF_TEXT, ptr1) == NULL)
 {
   message("failure setting clipboard");
    GlobalFree(ptr1);
    return -1;
 }
 CloseClipboard();
 GlobalFree(ptr1);
message("Placed %d bytes on clipboard",filelen);
return 0;
#endif
}

call(pastclipboard)
call(toclipboard)
