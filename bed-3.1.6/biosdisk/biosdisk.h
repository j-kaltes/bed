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
#ifdef WIN32
#ifndef LONGLONG
#define LONGLONG long long
#endif
extern WORD  diskread(Diskinfo *disk,void *buf,BLOCKCOUNT count, LONGLONG offset );
extern BOOL  diskwrite(Diskinfo *disk,void *buf,BLOCKCOUNT count, LONGLONG offset );
extern LONGLONG getfilesize(Diskinfo *disk) ;
extern int getblocksize(Diskinfo *disk) ;
extern Diskinfo *diskopen(int drive,int access) ;
extern Diskinfo *diskreopen(Diskinfo *,int access) ;
extern int diskclose(Diskinfo *drive);
extern BOOL _export diskpresent(int drive);
#endif
#ifdef  __cplusplus
	}
#endif
#endif
