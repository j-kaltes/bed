/*
Calls thunk16.dll by  QT_Thunk
The principles I found in the file QTThunkU.pas in the source of explore2fs
(http://uranus.it.swin.edu.au/~jn/linux/)

Fri May 24 21:38:19  2002
Jaap Korthals Altes <binaryeditor@gmx.com>
*/
#include <stdio.h>
#include <windows.h>
#include <sys/fcntl.h>
#include "biosdisk.h"
#include "biosthun.h"
#ifdef DODEBUG
#define DEBUG(x...) fprintf(stderr,x);
#else
#define DEBUG(x...)
#endif


#ifdef __GNUC__
#define startstack {char eatspace[0x3C]="";
//#define startstack {
#define setproc(proc) asm("movl   %0,%%edx":: "m"(proc));
/*
set edx to proc
e.g	movl   _testargs16,%edx
*/
#define pushchar(one) pushword(one)
#define pushword(one) asm("mov   %0,%%ax\n	push %%ax":: "m"(one));
/*
push word size on stack
e.g.
	mov   -2(%ebp),%ax
	push %ax
*/
#define pushdword(one) asm("pushl   %0":: "m"(one));
//e.g:	pushl   12(%ebp)
#define THUNK asm("call _QT_Thunk");
#define getres(get)\
	asm("mov   %%ax,%0":"=m"((((WORD*)&(get))[0])));\
	asm("mov   %%dx,%0":"=m"(((WORD*)&(get))[1]));}
/*
get get out of ax and dx, e.g:
	mov   %ax,-8(%ebp)
	mov   %dx,-6(%ebp) */
#else
define the following procedures:
 setproc(proc)
 pushchar(one)
 pushword(one) 
 pushdword(one)
 THUNK 
 getres(get)
#pragma message ( "****\nDon't know inline assembler format for your compiler\n****")
#endif
#define call16(proc,res) setproc(proc);THUNK;getres(res);
LPVOID WINAPI WOWGetVDMPointerFix(DWORD vp, DWORD dwBytes,
                                  BOOL fProtectedMode);
LPVOID WINAPI WOWGetVDMPointer(DWORD vp, DWORD dwBytes,
                               BOOL fProtectedMode);
VOID WINAPI WOWGetVDMPointerUnfix(DWORD vp);
WORD  WINAPI WOWGlobalAlloc16(WORD wFlags, DWORD cb);
WORD  WINAPI WOWGlobalFree16(WORD hMem);
DWORD WINAPI WOWGlobalLock16(WORD hMem);
BOOL  WINAPI WOWGlobalUnlock16(WORD hMem);

static BOOL  GetDriveParameters(Diskinfo *diskd); 
/*
HGLOBAL WINAPI GlobalAlloc16(UINT flags,DWORD bytes);
HGLOBAL WINAPI GlobalFree16( HGLOBAL); 
void WINAPI FAR* GlobalLock16(HGLOBAL hglb);
BOOL WINAPI GlobalUnlock16(HGLOBAL);
*/
/*
typedef HINSTANCE   FAR WINAPI (*LoadType)(LPCSTR);
static HINSTANCE   FAR WINAPI (*LoadLibrary16)(LPCSTR);
typedef void WINAPI (*FreeLibraryType)(HINSTANCE);
static void WINAPI (*FreeLibrary16)(HINSTANCE);
typedef FARPROC WINAPI (*GetProcAddressType)(HINSTANCE,LPCSTR);
static FARPROC WINAPI (*GetProcAddress16)(HINSTANCE,LPCSTR);
typedef void WINAPI (*QT_ThunkType)(void);
static void WINAPI (*QT_Thunk)(void);
*/
extern HINSTANCE   FAR WINAPI LoadLibrary16(LPCSTR);
extern void WINAPI FreeLibrary16(HINSTANCE);
extern FARPROC WINAPI GetProcAddress16(HINSTANCE,LPCSTR);
extern void WINAPI QT_Thunk(void);

void  dosfree(unsigned short);
DWORD dosalloc(DWORD);

FARPROC dosalloc16,dosfree16,diskwrite16, diskread16,diskgegs16;
HINSTANCE hinstance16=0;
extern void _export freelib16(void) {
	if(hinstance16) {
		FreeLibrary16(hinstance16);
		hinstance16=0;
		}
	}
#define dll16 "biosdisk.dll"
#include "../src/winpad.h"
int loadlib16(void) {
  if(hinstance16) {
		return 0;
		}
  hinstance16=LoadLibrary16( CONFDIRWIN "\\" dll16);
 
		if(((UINT)hinstance16)<HINSTANCE_ERROR) {
		hinstance16=LoadLibrary16(dll16);
		if(((UINT)hinstance16)<HINSTANCE_ERROR) {
			hinstance16=0;
			fprintf(stderr,"loadlibrary16(%s) failed\n",dll16);
			return -1;
			}
		}
	dosalloc16=GetProcAddress16(hinstance16,"DOSALLOC");
	if(!dosalloc16) {
		fprintf(stderr,"GetProcAddress16(hinstance16,\"DOSALLOC\");\n");
		FreeLibrary16(hinstance16);
	  	hinstance16=0;
		return(-4);
		}
	dosfree16=GetProcAddress16(hinstance16,"DOSFREE");
	if(!dosfree16) {
		fprintf(stderr,"GetProcAddress16(hinstance16,\"DOSFREE\");\n");
		FreeLibrary16(hinstance16);
	  	hinstance16=0;
		return(-4);
		}
diskread16=GetProcAddress16(hinstance16,"DISKREAD");
	if(!diskread16) {
		fprintf(stderr,"GetProcAddress16(hinstance16,\"DISKREAD\");\n");
		FreeLibrary16(hinstance16);
	  	hinstance16=0;
		return(-4);
		}
diskwrite16=GetProcAddress16(hinstance16,"DISKWRITE");
	if(!diskwrite16) {
		fprintf(stderr,"GetProcAddress16(hinstance16,\"DISKWRITE\");\n");
		FreeLibrary16(hinstance16);
	  	hinstance16=0;
		return(-4);
		}
diskgegs16=GetProcAddress16(hinstance16,"GETDRIVEPARAMETERS");
	if(!diskgegs16) {
		fprintf(stderr,"GetProcAddress16(hinstance16,\"GETDRIVEPARAMETERS\");\n");
		FreeLibrary16(hinstance16);
	  	hinstance16=0;
		return -2;
	  }
atexit(freelib16);
return 0;
}
#ifdef MAIN
int main(void) 
{
{
Diskinfo *dat;
if( dat=diskopen(0x80,O_RDONLY)) {
	printf("%lld %ld\n", dat->disksectors, dat->BytesPerSector);
	}
else {
	printf("diskopen.*failed\n");
	exit(4);
	}


{
const int BLOCKSIZE=4096;
char buf[BLOCKSIZE];
long iter;
FILE *fp=fopen("middle","wb");
if(!fp) {
	fprintf(stderr,"CAn't open middle\n");
	  FreeLibrary16(hinstance16);
	exit(4);
	}
  if(diskread(dat,buf,(BLOCKSIZE+511)/512,4 )) 
	fwrite(buf,BLOCKSIZE,1,fp);
	else {
		fprintf(stderr,"diskread failed %ld\n",iter);
	  FreeLibrary16(hinstance16);
		fclose(fp);
		exit(9);
		}

	fclose(fp);
}
}
	  FreeLibrary16(hinstance16);
	
  }
#endif

void  dosfree(unsigned short sel) {
	long res;
	startstack;
	pushword(sel);
	setproc(dosfree16);
	THUNK;
	getres(res);
	}
DWORD dosalloc(DWORD size) {
	DWORD res;
	startstack;
	pushdword(size);
	setproc(dosalloc16);
	THUNK;
	getres(res);
	return res;
	}
LONGLONG _export getfilesize(Diskinfo *disk) {
	return disk->disksectors*getblocksize(disk); 
	}

/*asm (".section .drectve");
asm (".ascii \"-export:getblocksize\"");
*/

int _export getblocksize(Diskinfo *disk) {
int i;
for(i=0;i<100;i++)
		;
	return disk->BytesPerSector;
	}
Diskinfo* _export diskreopen(Diskinfo *info,int access) {
		if(access==O_RDONLY)
			info->readonly=1;
		else
			info->readonly=0;
		return info;	
	}
int _export removebioslib(void) {
	if(loadlib16()!=0) {
		return 0;
		}
	if(hinstance16) {
		int i;	
		for(i=0;i<20;i++)
			FreeLibrary16(hinstance16);
		hinstance16=0;
		return 1;
		}
	return 0;
	}
BOOL _export diskpresent(int drive) 
{
	BOOL res;
	Diskinfo *info;
	if(loadlib16()!=0) {
		return FALSE;
		}
	if(!(info=(Diskinfo *)malloc(sizeof(Diskinfo))))
			return FALSE;
	info->drive=drive;
	res=GetDriveParameters(info); 
	free(info);
	return res;
}
Diskinfo* _export diskopen(int drive,int access) {
{
Diskinfo *info;
if(loadlib16()!=0) {
	return NULL;
	}
	info=(Diskinfo *)malloc(sizeof(Diskinfo));
	if(!info) {
		fprintf(stderr,"malloc(%d) failed\n",sizeof(Diskinfo));
		return NULL;
		}
	info->drive=drive;
	if(GetDriveParameters(info) ) {
		unsigned char *buf;
		if(access==O_RDONLY)
			info->readonly=1;
		else
			info->readonly=0;
		info->BytesPerSector+=512; /*Maybe it was wrong before reading */
		if(!(buf=(unsigned char*)malloc(info->BytesPerSector))) {
				free(info);
				return NULL;
				}
		if(!diskread(info,buf,1,0LL )) {
				free(info);
				free(buf);
				return NULL;
				}
		free(buf);
		if(GetDriveParameters(info) ) 
			return info;	
		}
	free(info);
	return NULL;
}
	}
static DWORD diskgegs(DWORD info) 
  {
	DWORD res;
	startstack;
	pushdword(info);
	call16(diskgegs16,res);
	return res;
  };

static BOOL  GetDriveParameters(Diskinfo *diskd) 
{
  WORD hgl;
DWORD  info;
  DWORD res;
Diskinfo *bli;
  if((hgl=WOWGlobalAlloc16(GMEM_FIXED,sizeof(struct diskinfo)))==0) 
	{
		fprintf(stderr, "WOWGlobalAlloc16 failed\n"); return FALSE;
	}
  if((info=WOWGlobalLock16(hgl))==0) 
	{
		fprintf(stderr,"Globallock16 failed\n");
		WOWGlobalFree16(hgl);
		return FALSE;
	}
bli=WOWGetVDMPointerFix(info, sizeof(struct diskinfo), TRUE);
  if(!bli) {
	fprintf(stderr,"WOWGetVDMPointer failed\n");
  	WOWGlobalUnlock16(hgl);
  	WOWGlobalFree16(hgl);
	return FALSE;
	}
	bli->drive=diskd->drive;

res= diskgegs( info) ;
if(!res) {
  WOWGetVDMPointerUnfix(hgl);
  WOWGlobalUnlock16(hgl);
  WOWGlobalFree16(hgl);
	return FALSE;
	}
	memcpy(diskd,bli,sizeof(struct diskinfo));
	if(diskd->disksectors==0) 
		diskd->disksectors= diskd->cylinders*diskd->heads*diskd->sectors;
WOWGetVDMPointerUnfix(hgl);
  WOWGlobalUnlock16(hgl);
  WOWGlobalFree16(hgl);
return TRUE;	
}
int _export diskclose(Diskinfo *drive) {
if(drive)
	free(drive);
return 0;
}

 static WORD  dodiskread(DISKIDENT disk,DWORD Rbuf,BLOCKCOUNT count,DWORD low,DWORD hi ) 
{
DWORD res;
	startstack;
	pushword(disk);
	pushdword(Rbuf);
	pushword(count);
	pushdword(hi);
	pushdword(low);
	setproc(diskread16);
	THUNK;
	getres(res);
return res&0xFFFF;
}
	



BLOCKCOUNT _export diskread(Diskinfo *disk,void *buf,BLOCKCOUNT count, LONGLONG offset ) 
{
int blocksize=disk->BytesPerSector;
SIZE_T  size=count*blocksize;
DWORD Rbuf;    // Real-mode buffer pointer
DWORD Pbuf;    // Protected-mode buffer pointer
DWORD han,low,hi;
BLOCKCOUNT retcount;
if(count<=0) {
	fprintf(stderr,"diskread with count=%d\n",count);
  return FALSE;
	}
if(offset<0LL) {
	fprintf(stderr,"diskread with offset=%lld\n",offset);
  return FALSE;
	}
if(!(han=dosalloc(size))) {
		return DISKREADFAILED;
		}
Rbuf = MAKELONG(0, HIWORD(han));
Pbuf = MAKELONG(0, LOWORD(han));
low=offset&0xFFFFFFFF;
hi=offset>>32;
//DEBUG("diskread(%hd,%lX,%hd,%ld,%ld)\n",disk->drive,Rbuf,count,hi,low);
retcount=dodiskread(disk->drive,Rbuf,count, low,hi );
if(retcount) {
	char *flatbuf=WOWGetVDMPointerFix(Pbuf, size, TRUE);
	if(!flatbuf) {
		fprintf(stderr,"diskread failed, invalid address\n");
		dosfree(LOWORD(han));
		return FALSE;
		}
	memcpy(buf,flatbuf,size);
	WOWGetVDMPointerUnfix(Pbuf);
	}
dosfree(LOWORD(han));
return retcount;
}

static short  dodiskwrite(DISKIDENT disk,DWORD Rbuf,BLOCKCOUNT count,DWORD low,DWORD hi ) 
{
long res;
	startstack;
	pushword(disk);
	pushdword(Rbuf);
	pushword(count);
	pushdword(hi);
	pushdword(low);
	setproc(diskwrite16);
	THUNK;
	getres(res);
return res&0xFFFF;
}

BOOL _export diskwrite(Diskinfo *disk,void *buf,BLOCKCOUNT count, LONGLONG offset ) 
{
if(disk->readonly) {
	fprintf(stderr,"diskwrite: Disk opened readonly\n");
	return FALSE;
	}
if(count<=0) {
	fprintf(stderr,"diskwrite with count=%d\n",count);
  return FALSE;
	}
if(offset<0LL) {
	fprintf(stderr,"diskwrite with offset=%lld\n",offset);
  return FALSE;
	}
{
int blocksize=disk->BytesPerSector;
SIZE_T  size=count*blocksize;
DWORD Rbuf;    // Real-mode buffer pointer
DWORD Pbuf;    // Protected-mode buffer pointer
DWORD han,low,hi;
BOOL res;
if(!(han=dosalloc(size))) {
		fprintf(stderr,"diskwrite: dosalloc(%lu) failed\n",size);
		return DISKREADFAILED;
		}
Rbuf = MAKELONG(0, HIWORD(han));
Pbuf = MAKELONG(0, LOWORD(han));
{
char *flatbuf=WOWGetVDMPointerFix(Pbuf, size, TRUE);
if(!flatbuf) {
	dosfree(LOWORD(han));
	fprintf(stderr,"diskwrite: WOWGetVDMPointerFix failed\n");
	return FALSE;
	}
memcpy(flatbuf,buf,size);
WOWGetVDMPointerUnfix(Pbuf);
}
low=offset&0xFFFFFFFF;
hi=offset>>32;
res=dodiskwrite(disk->drive,Rbuf,count, low,hi );
dosfree(LOWORD(han));
return res;
}
}

