#ifndef BIOSDISK_H
#define BIOSDISK_H
#include <windows.h>
#ifdef __GNUC__
#include <sys/fcntl.h>
#endif
//
#ifdef  __cplusplus
	extern "C" {
#endif

typedef WORD BLOCKCOUNT;
#ifdef __GNUC__
#define PACKSTRUCT __attribute__ ((packed))
#else
#define PACKSTRUCT
#pragma -a1
#endif
struct diskinfo {
	DWORD cylinders;
	DWORD heads;
	DWORD sectors;
#ifdef WIN32
	union {
		struct {
		  DWORD sectorsLo;
		  DWORD sectorsHi;
			};
	 	LONGLONG disksectors; 
		};
#else
		  DWORD sectorsLo;
		  DWORD sectorsHi;
#endif
	WORD BytesPerSector;
 	BYTE readonly;	
	BYTE drive;	
   } PACKSTRUCT;

typedef struct diskinfo Diskinfo;
#ifdef __GNUC__
#ifndef LONGLONG
#define LONGLONG long long
#endif
#endif
#ifdef _WIN32
typedef BOOL (*Diskread)(Diskinfo *disk,void *buf,BLOCKCOUNT count, LONGLONG offset );
extern Diskread diskread,diskwrite;

typedef LONGLONG (*Getfilesize)(Diskinfo *disk) ;
extern Getfilesize getfilesize,getblocksize;
typedef Diskinfo *(*Diskopen)(int drive,int access) ;
extern Diskopen diskopener;

extern Diskinfo *diskopen(int disknum,int access);
typedef Diskinfo *(*Diskreopen)(Diskinfo *,int access) ;
extern Diskreopen diskreopen;
typedef int (*Diskclose)(Diskinfo *drive);
extern Diskclose diskclose;
typedef  BOOL  (*Diskpresent)(int drive);
extern Diskpresent diskpresenter;
extern  BOOL  diskpresent(int drive);

#endif
#ifdef  __cplusplus
	}
#endif
#endif
