#pragma once

#include "offtypes.h"
#ifdef HAVE_SENDFILE64
#define SENDFILE sendfile64
	#ifdef HAVE_OFF64_T 
		#define OFFHIER off64_t
	#else
	#ifdef HAVE____OFF64_T 
		#define OFFHIER __off64_t
	#else
		#define OFFHIER __off_t
	#endif
	#endif
#else
#define SENDFILE sendfile
	#define OFFHIER off_t 
#endif
extern int nonsparsecp(FILEHANDLE inhandle,OFFHIER instart,OFFHIER inlen,FILEHANDLE uit) ;
//extern int sparsecp(FILEHANDLE inhandle,FILEHANDLE uit);
extern int sparsecp(int inhandle,OFFHIER instart,OFFHIER inlen,int uit) ;
