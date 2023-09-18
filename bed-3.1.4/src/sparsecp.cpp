
#define _GNU_SOURCE 1

//#define TEST
#define  INCLS
#ifdef  INCLS

#include "defines.h"
#endif
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "sparsecp.h"

#if defined(TEST)|| (defined(HAVE_SENDFILE) && defined(SEEK_DATA))

#include <stdarg.h>

#include <sys/sendfile.h>
 
#ifdef INCLS
#include "offtypes.h"
#else
#define FTRUNCATE ftruncate
#endif
#include "sparsecp.h"
//extern  void *memcpy(void *dest, const void *src, size_t n);


//#include <type_traits>

void copyother(char *dest,const char *orig) {
	if(dest!=orig)
		strcpy(dest,orig);
	}
void copyother(char *dest,int get) {
	}


void error(const char *format, ...) {
	int waserrno=errno;
	constexpr const int buflen=256;
	char buf[buflen];
        va_list args;
        va_start(args, format);
	int len=vsnprintf(buf,buflen,format,args);
        va_end(args);
	if(len<buflen) {
		memcpy(buf+len,": ",2);
		len+=2;
		char *str=buf+len;
		auto newstr=strerror_r(waserrno,str, buflen-len);
		copyother(str,newstr);
		/*
		if constexpr ( std::is_same<typename std::decay<decltype(newstr)>::type, char *>::value) {
			const char *nonsense=(const char *) newstr;
			if(nonsense!=str)
				strcpy(str,nonsense);
			}
			*/
		}
	fputs(buf,stderr);
        }




int sparsecp(int inhandle,int uit) {
   OFFHIER hole=0; 
   for(OFFHIER data;(data = LSEEK(inhandle, hole, SEEK_DATA))!=-1;) {
	if((hole = LSEEK(inhandle, data, SEEK_HOLE)) == -1) {
		error("lseek(," OFFPRINT ", SEEK_HOLE): %s",data);
		return -2;
		}
	for(OFFHIER pos = data; pos < hole;){
		OFFHIER len = hole - pos; 
		LSEEK(uit,pos,SEEK_SET);
		if((len=SENDFILE(uit,inhandle, &pos,len))<0) {
			error("sendfile(,," OFFPRINT "," OFFPRINT "): %s",pos,len);
			return -4;
			}
		if(!len)
			return 0;
		}
	  }
	if(errno == ENXIO) 
		return 0;
	if(hole==0)
		return -1;
	error("lseek(," OFFPRINT ",SEEK_DATA)",hole);
	return -3;
}
int nonsparsecp(int inhandle,OFFHIER instart,OFFHIER inlen,int uit) {
	OFFHIER end=instart+inlen;
	for(OFFHIER pos = instart;pos<end;){
		OFFHIER len=end-pos;
		if((len=SENDFILE(uit,inhandle, &pos,len))<0) {
			error("sendfile(,," OFFPRINT "," OFFPRINT ")",pos,len);
			return -4;
			}
		if(!len)
			return 0;
		}
	return 0;
	}

int sparsecp(int inhandle,OFFHIER instart,OFFHIER inlen,int uit) {
/*	if(FTRUNCATE(uit,inlen)!=0) {
		perror("ftruncate");
		return -5;
		}
		*/
	OFFHIER inend=instart+inlen,hole=instart;
   for(OFFHIER data;(data = LSEEK(inhandle, hole, SEEK_DATA))!=-1;) {
	if(data>=inend)
		return 0;
	if((hole = LSEEK(inhandle, data, SEEK_HOLE)) == -1) {
		error("lseek(," OFFPRINT " SEEK_HOLE)",data);
		return -2;
		}
	if(hole>=inend)
		hole=inend;
	for(OFFHIER pos = data; pos < hole;){
		OFFHIER len = hole - pos; 
		OFFHIER posuit=pos-instart;
		LSEEK(uit,posuit,SEEK_SET);
		if((len=SENDFILE(uit,inhandle, &pos,len))<0) {
			error("sendfile(,," OFFPRINT "," OFFPRINT ")",pos,len);
			return -4;
			}
		if(!len)
			return 0;
		}
	   if(hole==inend)	
	   	return 0;
	  }
	if(errno == ENXIO) 
		return 0;
	if(hole==instart)
		return -1;
	error("lseek(," OFFPRINT ",SEEK_DATA)",hole);
	return -3;
}

#ifdef TEST
    #include <sys/types.h>
       #include <sys/stat.h>

int main(int argc,char **argv){
	const char *infilename=argv[1],*uitfilename=argv[2];
	int inhandle=open(infilename,O_RDONLY);
	if(inhandle<0) {
		error("open(%s,)",infilename);
		return 2;
		}
	int uit=creat(uitfilename,0700);
	if(uit<0) {
		error("creat(%s..)",uitfilename);
		return 3;
		}
       //	sparsecp(inhandle,uit);
       struct stat st;
       if(fstat(inhandle,&st)!=0) {
       		error("fstat");	
		return 4;
       		}
       //	sparsecp(inhandle,0,st.st_size,uit);
       OFFHIER off=5446744073708L;

       	//nonsparsecp(inhandle,4096,5000,uit);
//       	sparsecp(inhandle,0,st.st_size,uit);
 sparsecp( inhandle, uit) ;
}
#endif
#endif

