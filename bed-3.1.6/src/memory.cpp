#include "defines.h"
/*     Bed a Binary EDitor for Linux and for Rxvt running under Linux.       */
/*     Copyright (C) 1998  Jaap Korthals Altes <binaryeditor@gmx.com>      */
/*                                                                           */
/*     Bed is free software; you can redistribute it and/or modify           */
/*     it under the terms of the GNU General Public License as published by  */
/*     the Free Software Foundation; either version 2 of the License, or     */
/*     (at your option) any later version.                                   */
/*                                                                           */
/*     Bed is distributed in the hope that it will be useful,                */
/*     but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*     GNU General Public License for more details.                          */
/*                                                                           */
/*     You should have received a copy of the GNU General Public License     */
/*     along with bed; if not, write to the Free Software                    */
/*     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.             */
/*                                                                           */
/* Sun Dec  6 18:34:27 1998                                                  */


#define DEBUGUIT(...) //fprintf(stderr,__VA_ARGS__);
#include "desturbe.h"
#include <features.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdint.h>
#include <assert.h>
#ifdef __linux__
#include <linux/fs.h>
#if !(defined(__LITTLE_ENDIAN_BITFIELD) || defined(__BIG_ENDIAN_BITFIELD))
#define __LITTLE_ENDIAN_BITFIELD 1
#endif
#include <linux/cdrom.h>
#endif
#ifdef  HAVE_SYS_IOCTL_H 
#include <sys/ioctl.h>
#endif
#include <limits.h>
#include <unistd.h>
#include <ctype.h>
#include "memory.h"
#include "system.h"
#include "myalloc.h"
#include "main.h"
#include "misc.h"
#ifdef USE_WINFILE
#include <windows.h>
#include <winioctl.h>
#include "win.h"
#include "winsparsecp.h"
#endif

extern const char NEWFILENAME[];
extern const int newfilenamelen;
const char NEWFILENAME[]="Untitled";
const int newfilenamelen=sizeof(NEWFILENAME)-1;

//s/sectoralloc(\([^;]*;\)$/alignedalloc(blocksize(),\1/g
//s/sectorfree\(.*\)$/alignedfree\1/g

#define databuffree(x) alignedfree(x)
#define databufalloc(size) (char *)alignedalloc(BLOCKSIZE, size)

int userblocksize=-1;


OFFTYPE setfilepos(FILEHANDLE file,OFFTYPE offset) ;


#ifdef NEEDS_BLOCKMODE
inline BOOL Memory::sizehigher(OFFTYPE sizefile) {
if(blockreadonly>1) {
	char *blk=alignedalloc(blocksize(),BLOCKSIZE);
	OFFTYPE offunder=(sizefile/blockreadonly)*blockreadonly;
	BEDS_SIZE_T  got=realblockreadat(blk,BLOCKSIZE,offunder);
	alignedfree(blk);
	return (offunder+got)>sizefile;
}
else {
	char readtestchar;
	return (realblockreadat(&readtestchar,1,(sizefile))>=1);
        }
}
#else
inline BOOL Memory::sizehigher(OFFTYPE sizefile) {
	char readtestchar;
	return (realblockreadat(&readtestchar,1,(sizefile))>=1);
	}
#endif
//#define sizehigher(sizefile) (readat(&readtestchar,1,(sizefile))>=1)

/*slaat nergens op in blocks*/
inline BOOL Memory::testsize(OFFTYPE sizetest) {
	return (sizehigher((sizetest))?0:((sizehigher(((sizetest)-1)))?1:0));
	}
#define MAXCDSIZEDEF (20LL*1024*1024*1024LL)
extern OFFTYPE MAXCDSIZE;
#if OFFTYPEMAX> MAXCDSIZEDEF
OFFTYPE MAXCDSIZE = MAXCDSIZEDEF;
#else
OFFTYPE MAXCDSIZE = OFFTYPEMAX;
#endif
char realsize=0;


#ifndef NEEDS_BLOCKMODE
void Memory::exploresize(FILEHANDLE file,OFFTYPE begin, OFFTYPE end) {
	OFFTYPE half;
	do {
		half=(OFFTYPE)((LARGERTYPE)(end+begin)/2);
		if(sizehigher((half)))
			begin=half+1;
		else
			end=half;
		} while(end>begin);
	filesize=begin;
	if(filesize>= MAXDISCSIZE)
		errors=6;
	}
#else
void Memory::exploresize(FILEHANDLE file,OFFTYPE begin, OFFTYPE end) {
	OFFTYPE half;
	OFFTYPE beginblock=begin/(LARGERTYPE)blockreadonly;
	OFFTYPE endblock=end/(LARGERTYPE)blockreadonly;
	do {
		half=(OFFTYPE)((LARGERTYPE)(endblock+beginblock)/2);
		if(sizehigher((half*blockreadonly)))
			beginblock=half+1;
		else
			endblock=half;
		} while(endblock>beginblock);

	filesize= beginblock*blockreadonly;
	if(filesize>= MAXDISCSIZE)
		errors=6;
	}
#endif
//#define xEXPLORESIZE {if((filesize=exploresize(filehandle,begin,end))>=MAXDISCSIZE) { errors=6;}; return;}
//s/EXPLORESIZE/return exploresize(filehandle,begin,end)/g
OFFTYPE sizespecified=0;
#ifdef USE_WINFILE
FILEHANDLE opencreate(const char *filename) {
return CreateFile(
    filename,	// address of name of the file 
    GENERIC_WRITE,	// access (read-write) mode 
    FILE_SHARE_READ|FILE_SHARE_WRITE,	// share mode 
    NULL,	// address of security descriptor 
    CREATE_ALWAYS,	// how to create 
    FILE_ATTRIBUTE_NORMAL,	// file attributes 
    NULL 	// handle of file with attributes to copy  
   );
	}

FILEHANDLE openreadwrite(const char *filename) {
FILEHANDLE handleuit= CreateFile(
    filename,	// address of name of the file 
    GENERIC_READ|GENERIC_WRITE,	// access (read-write) mode 
//	FILE_SHARE_READ,
    FILE_SHARE_READ|FILE_SHARE_WRITE,	// share mode 
    NULL,	// address of security descriptor 
    OPEN_EXISTING,	// how to create 
    FILE_ATTRIBUTE_NORMAL,	// file attributes 
    NULL 	// handle of file with attributes to copy  
   );

return handleuit;
	}


FILEHANDLE openreading(const char *filename) {
extern char messagebuf[];
extern const int maxmessagebuf;
FILEHANDLE handleuit=CreateFile(
	  filename,	// address of name of the file 
	  GENERIC_READ,	// access (read-write) mode 
	  FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE
,	// share mode 
	  NULL,	// address of security descriptor 
	  OPEN_EXISTING,	// how to create 
//	  FILE_ATTRIBUTE_READONLY | FILE_FLAG_NO_BUFFERING 
//	 FILE_ATTRIBUTE_READONLY 
FILE_ATTRIBUTE_NORMAL
//FILE_FLAG_RANDOM_ACCESS
//0 
,	// file attributes 
	  NULL 	// handle of file with attributes to copy  
	 );

if(handleuit==INVALID_HANDLE_VALUE)
{

 winerror(messagebuf, maxmessagebuf );
/*
    DWORD dw = GetLastError(); 
    FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_MAX_WIDTH_MASK,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &messagebuf,
        maxmessagebuf, NULL );
*/
}
/*
gettimeofday(&tvalend,NULL);
timersub(&tvalend, &tvalstart, &tvaldiff);
fprintf(stderr,"%s: sec=%ld, usec=%ld\n",filename,tvaldiff.tv_sec ,tvaldiff.tv_usec);
*/

return handleuit;
	}
#else 

#ifndef HAVE_TRUNCATE64
static int mytruncate64(const char * const filename,OFFTYPE filesize) {
	FILEHANDLE han=LOPEN(filename,O_WRONLY LARGEFILE_FLAG BINARY_FLAG);
	if(han<0)
			return han;
	int ret=FTRUNCATE(han,filesize);
	LCLOSE(han);
	return ret;
	}
#endif
FILEHANDLE openreading(const char *filename) {
	return LOPEN(filename,O_RDONLY LARGEFILE_FLAG BINARY_FLAG);
	}
FILEHANDLE openreadwrite(const char *filename) {
  return LOPEN(filename,O_RDWR LARGEFILE_FLAG BINARY_FLAG);
	}

FILEHANDLE opencreate(const char *filename) {
  return LOPEN(filename,O_WRONLY|O_CREAT|O_TRUNC		 LARGEFILE_FLAG BINARY_FLAG,0600);
	}
#endif
/*
Returns -1 if no diskdevice, otherwise a bios disk identifier 
*/
int isdiskdevice(char *filename) 
{
int disknum=-1;
char start[]="\\\\.\\";
char disk[]="PHYSICALDRIVE";
//const int disklen=sizeof(disk)-1;
const int disklen=strlen(disk);
if(!strncmp(filename,start,sizeof(start)-1)) 
  {
  char *ptr=filename+sizeof(start)-1;
  if(ptr[2]=='\0'&&ptr[1]==':') {
		if((ptr[0]>='a'&&ptr[0]<='z'))	 {
			disknum=ptr[0]-'a';
			}
  else	{
			if(ptr[0]>='A'&&ptr[0]<='Z') {
				disknum=ptr[0]-'A';
				}
			}

			}
  else {
	  if(!strncasecmp(disk,ptr,disklen)) {
		  char *endp;
		  disknum=strtol(ptr+disklen, &endp,10);
		  if(!endp	||!*endp) {
			  disknum|=0x80;
			  }

		  }
	  }
  }
return disknum;
}

#ifdef BIOSDISK

extern bool doswin(void);

#include "winpad.h"
Diskread diskread=NULL,diskwrite=NULL;
Getfilesize getfilesize=NULL,getblocksize=NULL;
Diskopen diskopener=NULL;
Diskreopen diskreopen=NULL;
Diskclose diskclose=NULL;
Diskpresent diskpresenter=NULL;

#define BIOSDISKDLL "biosdisk32.dll"
HINSTANCE biosdiskhinstance=NULL;
void freebiosdisk(void) {
	if(biosdiskhinstance){
			FreeLibrary(biosdiskhinstance);
			biosdiskhinstance=NULL;

			DEBUGGING("freebiosdisk ")
			}
	}
BOOL loadbiosdisk(void) {
	if(biosdiskhinstance)
		return 1;
//	if( !(biosdiskhinstance=LoadLibrary(CONFDIRWIN "//" BIOSDISKDLL)))
	if( !(biosdiskhinstance=LoadLibrary(CONFDIRWIN "\\" BIOSDISKDLL)))
		if(!(biosdiskhinstance=LoadLibrary(BIOSDISKDLL))) {
			fprintf(stderr,"LoadLibrary(%s) failed\n",BIOSDISKDLL);
			return 0;
			}
	atexit(freebiosdisk);
	if(!(diskopener=(Diskopen)GetProcAddress(biosdiskhinstance,"diskopen"))) {
		fprintf(stderr,"GetProcAddress(%s,diskopen) failed\n",BIOSDISKDLL);
		freebiosdisk();
		return 0;
		}
	if(!(diskread=(Diskread)GetProcAddress(biosdiskhinstance,"diskread"))) {
		fprintf(stderr,"GetProcAddress(%s,diskread) failed\n",BIOSDISKDLL);
		freebiosdisk();
		return 0;
		}
	if(!(diskwrite=(Diskread)GetProcAddress(biosdiskhinstance,"diskwrite"))) {
		fprintf(stderr,"GetProcAddress(%s,diskwrite) failed\n",BIOSDISKDLL);
		freebiosdisk();
		return 0;
		}
	if(!(diskclose=(Diskclose)GetProcAddress(biosdiskhinstance,"diskclose"))) {
		fprintf(stderr,"GetProcAddress(%s,diskclose) failed\n",BIOSDISKDLL);
		freebiosdisk();
		return 0;
		}
	if(!(getblocksize=(Getfilesize)GetProcAddress(biosdiskhinstance,"getblocksize"))) {
		fprintf(stderr,"GetProcAddress(%s,getblocksize) failed %ld\n",BIOSDISKDLL,GetLastError());
		freebiosdisk();
		return 0;
		}
	if(!(diskreopen=(Diskreopen)GetProcAddress(biosdiskhinstance,"diskreopen"))) {
		fprintf(stderr,"GetProcAddress(%s,diskreopen) failed\n",BIOSDISKDLL);
		freebiosdisk();
		return 0;
		}
	if(!(getfilesize=(Getfilesize)GetProcAddress(biosdiskhinstance,"getfilesize"))) {
		fprintf(stderr,"GetProcAddress(%s,getfilesize) failed\n",BIOSDISKDLL);
		freebiosdisk();
		return 0;
		}
	if(!(diskpresenter=(Diskpresent)GetProcAddress(biosdiskhinstance,"diskpresent"))) {
		fprintf(stderr,"GetProcAddress(%s,diskpresenter) failed\n",BIOSDISKDLL);
		freebiosdisk();
		return 0;
		}
	DEBUGGING("loadbiosdisk slaagt ")
	return 1;
	}
BOOL diskpresent(int disknum) {
	if( !loadbiosdisk() ) {
			return FALSE;
			}
try {
#ifdef DEBUGON
	DEBUGGING("try diskpresenter(%d).. ",disknum)
		BOOL ret= diskpresenter(disknum);
if(ret)
	DEBUGGING("slaagt ")
else
	DEBUGGING("faalt ")
	return ret;
#else
		return diskpresenter(disknum);
#endif
}
catch(...) {
fprintf(stderr,"Error testing biosdisk %d\n",disknum);
return FALSE;
}
		}
 
Diskinfo *diskopen(int disknum,int access) {
	if( !loadbiosdisk() ) {
			return NULL;
			}
	return diskopener(disknum,access);
	}
	
struct regdisk {
	DWORD cylinders;
	DWORD heads;
	DWORD sectors;
	LONGLONG disksectors; 
	WORD BytesPerSector;
	LONGLONG disksize;
   }  PACKSTRUCT;
typedef struct regdisk RegDisk;
char registrysubkey[]="SOFTWARE\\Bed\\harddrives";
OFFTYPE Memory::getregistrysize(Diskinfo *infodisk,OFFTYPE &begin,OFFTYPE &end) 
{
	HKEY hkProtocol;
//	memset(&statdata,'\0',sizeof(statdata));
//	memset(&FileInfo,'\0',sizeof(FileInfo));
	FileInfo.nNumberOfLinks=0;
	if((blockreadonly=getblocksize(infodisk))<=0)  {
		fprintf(stderr,"Zero Blocksize\n");
		return OFFTYPEINVALID;
		}
	
	filehandle=GetStdHandle(STD_ERROR_HANDLE); /*Don't point to anyone */

	if((infodisk->drive&0x80)) {
		int disknr=infodisk->drive&0x0F;
		char name[10]="disk";
		OFFTYPE cylen= infodisk->heads*infodisk->sectors*infodisk->BytesPerSector;
		filesize=getfilesize(infodisk);
		end=filesize+cylen;
		sprintf(name+4,"%d",disknr);
		DWORD bintype=REG_BINARY,dwType	;
		RegDisk diskgegs;
		if(RegOpenKeyEx( HKEY_LOCAL_MACHINE, registrysubkey,
				0, 
					KEY_ALL_ACCESS	,	// desired security access 
					&hkProtocol	// address of buffer for opened handle  
				 )==ERROR_SUCCESS) 
		{
		DWORD diskgegslen=sizeof(diskgegs);
		if(RegQueryValueEx(
				hkProtocol,	// handle of key to query 
				name,	// address of name of value to query 
				0,
				&dwType,	// address of buffer for value type 
				(BYTE *)&diskgegs,	// address of data buffer 
				&diskgegslen 	// address of data buffer size 
			 )==ERROR_SUCCESS) {
			 if(dwType==bintype&&diskgegslen==sizeof(diskgegs)&&
				!memcmp(&diskgegs,infodisk,((char*)&diskgegs.disksize)-((char *)&diskgegs.cylinders))) {

		;
		 			RegCloseKey(hkProtocol);
					filesize=diskgegs.disksize;
					if(!realsize)
							return begin=end=filesize;

					}
				}
		 RegCloseKey(hkProtocol);
		}
	/*To large end doesn't fail anymore, so try something which works
	with my three harddisks */
    if(sizehigher((end))) 
	  {
	  do {
		  end+=filesize;
		  if(end>=MAXDISCSIZE) {
			  break;
			  }
		  } while(sizehigher((end)));
	  };
		return filesize;
	}
#if 1
else 
	{
		if(realsize)
		{
			filesize=getfilesize(infodisk);
			begin=0;end=2*filesize;
			return filesize;
		}	
		else 
			return filesize=begin=end=getfilesize(infodisk);
	}
#else
else	 { /*Determine floppy size */
		const OFFTYPE highsize=2880*512,doubsize=1440*512;
		end=getfilesize(infodisk);
    if(end>=highsize&&sizehigher((highsize-1))) {
				if(end==highsize||!sizehigher(highsize))
					return filesize=begin=end=highsize;
				else	{
					begin=highsize+1;
					}
				}
		else {
			fprintf(stderr,"end="OFFPRINT " highsize=" OFFPRINT,end,highsize);
				if(sizehigher(doubsize-1)) {
						if(!sizehigher(doubsize))
							return filesize=begin=end=doubsize;
						else	{
							begin=doubsize-1;
							end=highsize-1;
							}

						}
				else {
					end=doubsize-1;
					}
				}

	return 0;
	}
#endif
}
BOOL Memory::setregistrysize(Diskinfo *infodisk,OFFTYPE filesize) 
{
if(!(infodisk->drive&0x80))
	return TRUE;

	HKEY hkProtocol;
	DWORD dispos;
	if(RegCreateKeyEx(
			HKEY_LOCAL_MACHINE,
			registrysubkey,
			0,	// reserved 
			"Geometry",	// address of class string 
			REG_OPTION_VOLATILE,	// special options flag 
			KEY_ALL_ACCESS	,	// desired security access 
			NULL,	// address of key security structure 
			&hkProtocol,	// address of buffer for opened handle  
			&dispos 	// address of disposition value buffer 
				 )==ERROR_SUCCESS) 
	 {
		RegDisk diskgegs;
		int disknr=infodisk->drive&0x0F;
		char name[10]="disk";
		sprintf(name+4,"%d",disknr);
		DWORD bintype=REG_BINARY;
		memcpy(&diskgegs,infodisk,(((char*)&diskgegs.disksize)-((char *)&diskgegs.cylinders)));
		diskgegs.disksize=filesize;
		if(RegSetValueEx(hkProtocol,
				name,	// address of value to set 
				0,
				bintype,	// flag for value type 
				(BYTE *)&diskgegs,	// address of value data 
				sizeof(diskgegs)	// size of value data 
			 )!=ERROR_SUCCESS) 
		{
			fprintf(stderr,"RegSetValueEx failed\n"); 
			return FALSE;
		}
		return TRUE;
	}
return FALSE;
}

#endif
#ifdef USE_WINFILE

inline bool set_dasd_io(HANDLE filehandle) {
	DWORD retsize;
return 	DeviceIoControl(
	  filehandle,              // handle to device
	  FSCTL_ALLOW_EXTENDED_DASD_IO,  // dwIoControlCode
	  NULL,                          // lpInBuffer
	  0,                             // nInBufferSize
	  NULL,                          // lpOutBuffer
	  0,                             // nOutBufferSize
		&retsize,     // number of bytes returned
	  NULL   // OVERLAPPED structure
	);
	}

inline FILEHANDLE Memory::openreading(const char *filename) {
FILEHANDLE handleuit=::openreading(filename);
#ifdef USE_WINFILE
if(used_dasd_io) {
	  set_dasd_io( handleuit);
	}
#endif
return handleuit;
}

inline FILEHANDLE Memory::openreadwrite(const char *filename) {
FILEHANDLE handleuit= :: openreadwrite(filename);
#ifdef USE_WINFILE
if(used_dasd_io) {
	  set_dasd_io( handleuit);
	}
#endif
return handleuit;
}



#define TOLONGLONG(largeint) (((largeint).LowPart)|(((long long)((largeint).HighPart))<<32))
extern OSVERSIONINFO WindowsVersion;
OFFTYPE Memory::win32diskfilesize(FILEHANDLE filehandle,OFFTYPE *begin,OFFTYPE *end) {
if(WindowsVersion.dwOSVersionInfoSize&&WindowsVersion.dwPlatformId!=VER_PLATFORM_WIN32_WINDOWS) {
	OFFTYPE abegin=0,anend=MAXDISCSIZE;

	DWORD retsize;
#warning "FSCTL_ALLOW_EXTENDED_DASD_IO doesn't work for PhysicalDrive0 etc. Check on what?"
if(set_dasd_io(filehandle))
	used_dasd_io=true;
  DISK_GEOMETRY diskgeo;
    if(DeviceIoControl(filehandle, IOCTL_DISK_GET_DRIVE_GEOMETRY
		, NULL, 0, &diskgeo, sizeof(diskgeo), &retsize,NULL)) {
	abegin=TOLONGLONG(diskgeo.Cylinders)*diskgeo.TracksPerCylinder*
	   diskgeo.SectorsPerTrack*diskgeo.BytesPerSector;

#ifndef NEEDS_BLOCKMODE
int
#endif
	blockreadonly=diskgeo.BytesPerSector;
	BLOCKSIZE=((BLOCKSIZE-1)/blockreadonly+1)*blockreadonly;
	}
if(WindowsVersion.dwMajorVersion>5||( WindowsVersion.dwMajorVersion==5&& WindowsVersion.dwMinorVersion>0)) {
  	GET_LENGTH_INFORMATION devlen;
	if(DeviceIoControl(
		  filehandle,              // handle to device
		  IOCTL_DISK_GET_LENGTH_INFO,    // dwIoControlCode
		  NULL,                          // lpInBuffer
		  0,                             // nInBufferSize
		  (LPVOID) &devlen,          // output buffer
		  (DWORD) sizeof(devlen),        // size of output buffer
		  (LPDWORD) &retsize,     // number of bytes returned
		  NULL    // OVERLAPPED structure
			)){

			if(realsize) {
				anend= devlen.Length.QuadPart;

				}	
			else
				return devlen.Length.QuadPart;

		}
	}
	else {
	PARTITION_INFORMATION parinfo;
    if(DeviceIoControl(filehandle, IOCTL_DISK_GET_PARTITION_INFO 

		, NULL, 0, &parinfo, sizeof(parinfo), &retsize,NULL)) {
	anend=(OFFTYPE)parinfo.PartitionLength.QuadPart;
	}
	}
	if(abegin>anend)
		myswap(abegin,anend);
	if(*begin<abegin) {
	  if(sizehigher(abegin-1)) {
	    if(!sizehigher(abegin))
		return abegin;
	    else
		*begin=abegin;
	   }
	 else {
		*end=abegin-1;
		return 0;
		}
	 }
	if(anend<*end) {
		if(!sizehigher(anend)) {
			if(sizehigher(anend-1))
				return anend;
			*end=anend-1;
			}
		else {
			if(!realsize)
				return anend;
			}
		}
	}

return 0;
}
#endif
int setblocksize(int bsize) {
	int BLOCKSIZE;
	if(userblocksize>0) {
		BLOCKSIZE=userblocksize;
		}
	else {
		if(bsize>STBLOCKSIZE)
			BLOCKSIZE=bsize;
		else
			BLOCKSIZE=STBLOCKSIZE;
		}
	return BLOCKSIZE;	
	}

#ifdef __FreeBSD__
int getblockreadonly(FILEHANDLE filehandle) 
{
	char readtestchar;
	int blockread=1;
	if(LPREAD(filehandle,&readtestchar,1,0)<1) 
	{
		int freebsdblocksize=1024;
		char buf[freebsdblocksize];
		if(LPREAD(filehandle,buf,freebsdblocksize,0)==freebsdblocksize) 
		{
			blockread=freebsdblocksize;
			LSEEK(filehandle,0,SEEK_SET);
		}
	}
	return blockread;
}
#endif
inline BOOL Memory::checkfilesize(OFFTYPE filsize,OFFTYPE &begin,OFFTYPE &end) 
{

	if(sizehigher(filsize)) {
		if(filsize>begin)
			begin=filsize;
		}
	else 
	{
		if(sizehigher(filsize-1)) 
			return TRUE;
		else {
			if(filsize<=end)
				end=filsize-1;
			}
	}
	return FALSE;
}	
#ifdef _WIN32
int Memory::getfileinfo( BY_HANDLE_FILE_INFORMATION *FInfo ) {
int ret;
if(!(ret=GetFileInformationByHandle(filehandle,FInfo ))) {
		FInfo->nNumberOfLinks=0;
		}
return ret;
}


/*
inline bool Memory::opendosdrive(const char *const filename,OFFTYPE &begin, OFFTYPE &end) {
int disknum;
if((disknum=isdiskdevice(filename))!=-1) 
  {
  if((infodisk=diskopen(disknum,O_RDONLY)))	
	{
	if((filesize=getregistrysize(infodisk,begin,end))==OFFTYPEINVALID) {
			diskclose(infodisk);infodisk=NULL;	
			errors=77;
			return;
			}
	if(begin==end)
		dontcheck=1;
	}
*/
#endif

#ifdef __linux__
bool Memory::getsizelinux(OFFTYPE &begin,OFFTYPE &end) {
const bool foundsize=true;
const bool notfoundsize=false;
		if(S_ISBLK(statdata.st_mode)) {
			int blkgetsize;
			if(ioctl(filehandle,BLKGETSIZE ,&blkgetsize)>=0) { 
				filesize= (OFFTYPE) blkgetsize*512;
				if(sizehigher(filesize)) 
					begin=filesize;
				else {
					if(sizehigher(filesize-1)) 
						return foundsize;
					else
						end=filesize-1;
					}
				}
		DEBUG("BLKGETSIZE failed\n");
/*Maybe a cdrom drive */
#ifdef CDROM_DISC_STATUS
		if(ioctl(filehandle,CDROM_DISC_STATUS)<0) {
				 return notfoundsize;
			}
		else { /*Cdrom */
				uint32_t pos[2];
//				char sbuf[8];
//				char *sbuf=reinterpret_cast<char *>(pos);
				char *sbuf=(char *)(pos);
				OFFTYPE lookat=0x8050;
				OFFTYPE newbegin,newend;

				if(readat(sbuf,8,lookat)>=8&&memcmp(sbuf,sbuf+4,4)) {
					for(int i=0;i<4;i++) {
						if(sbuf[i]!=sbuf[7-i]) {
							DEBUG("No opposite");
						return notfoundsize;
							}
						}
					newbegin=*pos*static_cast<OFFTYPE>(2048);
//					newbegin=*reinterpret_cast<uint32_t*>(&sbuf)*static_cast<OFFTYPE>(2048);
					if(sizehigher(newbegin-1)) {

						if(realsize&&sizehigher(newbegin)) {
							begin=newbegin;
							newend=MAXCDSIZE;
							if(sizehigher(newend)) 
								begin=newend;
							else {
								end=newend;
									 }
						return notfoundsize;
							}
						else {
							filesize=newbegin;

						return foundsize;
							}

						}
					else {
						end=newbegin; 
						}
					
					} /*End Read cdromsize */

						return notfoundsize;

			} /*END cdrom */
#else
			return notfoundsize;
#endif
		} /*END block device */

	return notfoundsize;
	}
#endif /*linux */
/*
typedef union _LARGE_INTEGER {
  struct {
    DWORD LowPart;
    LONG  HighPart;
  };
  struct {
    DWORD LowPart;
    LONG  HighPart;
  } u;
  LONGLONG QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;
*/
inline void Memory::regfilesize(void ) {
	#ifdef USE_WINFILE
filesize=((unsigned long long)FileInfo.nFileSizeHigh)<<32|FileInfo.nFileSizeLow;
if(filesize==0LL) {
#if 0
	((DWORD *)&filesize)[0]= GetFileSize(filehandle, ((DWORD *)&filesize)+1 );
#else
	if(!GetFileSizeEx( filehandle, reinterpret_cast<PLARGE_INTEGER>(&filesize)))
			filesize=0;
#endif
	}
#else
	filesize=statdata.st_size;
#endif
//fprintf(stderr,"Filesize="OFFPRINT"\n",filesize);
	}
#ifndef _WIN32
bool Memory::statmis() {
#ifdef PROCLOCKSBUG
	if(!strcmp(filename,"/proc/locks")) {
		errors=9;
		return true;
		}
#endif
	if(STAT(filename,&statdata)) {
#ifdef NOSTAT
		memset(&statdata,'\0',sizeof(statdata));
#else
		errors=8;	
		return true;
#endif
		}
	else
	if( S_ISFIFO(statdata.st_mode)  ) {
		errors=7;
		return true;
		}
return false;
	}
#endif
void Memory::openfile(void) 
{
OFFTYPE begin=0;
OFFTYPE end = MAXDISCSIZE;
filehandle=MY_INVALID_HANDLE_VALUE;
#ifdef BIOSDISK

if(doswin()) 
{
int disknum;
if((disknum=isdiskdevice(filename))!=-1) 
  {
  if((infodisk=diskopen(disknum,O_RDONLY)))	
	{
	if((filesize=getregistrysize(infodisk,begin,end))==OFFTYPEINVALID) {
			diskclose(infodisk);infodisk=NULL;	
			errors=77;
			return;
			}
	goto havesize;
   }
  }
}
{
#endif
#ifndef _WIN32
if(statmis())
	return;
#endif

if((filehandle=openreading(filename))==MY_INVALID_HANDLE_VALUE) {
	  errors=9;
	  return;
	}	
#ifdef __FreeBSD__
if(statdata.st_mode&S_IFBLK ) 
	blockreadonly=getblockreadonly(filehandle);
#endif


#ifdef _WIN32
getfileinfo(&FileInfo);
#endif
#ifdef BIOSDISK
}
havesize:

#endif

#ifdef _WIN32
BLOCKSIZE= setblocksize(STBLOCKSIZE );
#else
BLOCKSIZE=setblocksize(statdata.st_blksize);
#endif


#ifdef USE_WINFILE
if(filename[0]=='\\'

#ifdef BIOSDISK
&&!infodisk
#endif
) {
	filesize=win32diskfilesize(filehandle,&begin,&end) ;
}
#endif
if(sizespecified) {
	filesize=sizespecified;
	return;
	}

#ifdef BIOSDISK
if(infodisk) {
	if(begin==end&&!realsize) 
		return;
	if((filesize>0&&checkfilesize(filesize,begin,end))) 
					return;
 	return exploresize(filehandle,begin,end) ;
	}
else
#endif
#ifdef USE_WINFILE
{
if(filesize)
		return;
if(begin||(end!= MAXDISCSIZE)) {
 	return exploresize(filehandle,begin,end) ;
	}
}
#endif
#ifndef ALWAYSEXPLORESIZE
#ifdef _WIN32
if(FileInfo.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
{
	errors=5;
	return;
}
#else
if(S_ISREG(statdata.st_mode) ) 
{
#endif
mmapfailed=0;
regfilesize();
if(filesize>0&&checkfilesize(filesize,begin,end))
		return;
	goto catchall;
#ifndef _WIN32
}
else   
{

#ifdef __linux__
if(getsizelinux(begin,end)) 
	return;
#endif
} /*END not regular */
if(S_ISDIR(statdata.st_mode)) 
{
	errors=5;
	return;
}
else 
#endif // WIN32
#endif /*ALWAYSEXPLORESIZE*/
{
 catchall:
 return exploresize(filehandle,begin,end) ;
}
}
















































inline void Memory::initall(void) {
}
void Memory::init(const char *name) {
	initall();
	const int len=filenamelen+1;
	filename=myallocar( char,len+3);
	memcpy(filename,name,len);

	openfile();
#ifdef BIOSDISK
if(infodisk)
	setregistrysize(infodisk,filesize);
#endif
	diskfilesize=filesize;

	useddiskfilesize=diskfilesize;

#if defined(USETHREADS) && !defined(INTSEARCH)
//if(filesize>(OFFTYPE)40000000L)
//	lastforward|=0x10;
#endif
	}


int newfilesize=1024;
extern int newfilesize;

Memory::Memory(const char *name): 
#ifdef USE_WINFILE
	used_dasd_io(false),
#endif
#ifdef BIOSDISK
	infodisk(NULL),
#endif
#ifdef NEEDS_BLOCKMODE
blockreadonly(1),
#endif
errors(0),
//readonly(0),filesize(0), lastforward(1),filenamelen(strlen(name))
//readonly(0),filesize(0), lastforward(3),searchpos(0),filenamelen(name?strlen(name):0)
readonly(0),filesize(0), filenamelen(name?strlen(name):0)
{
	 INITUSELSEEK;
     if(name)
		init(name);
     else {
       initall();
	filesize=newfilesize;
	BLOCKSIZE=512;
	filehandle= MY_INVALID_HANDLE_VALUE;
#ifndef _WIN32
	memset((void *)&statdata,0,sizeof(statdata));
#endif
	}
}
void closefilehandle(FILEHANDLE file) {
  #ifdef USE_WINFILE
	  if(file!=MY_INVALID_HANDLE_VALUE) {
		  if(!CloseHandle(file)) {
			  fprintf(stderr,"Close failed\n");
			  }
		  }
  #else
	  if(file>0)
		  LCLOSE(file);
  #endif
	}
void Memory::endfile(void) {
#ifdef BIOSDISK
	if(infodisk) {
		diskclose(infodisk);
		infodisk=NULL;
		}
	else
#endif
	  closefilehandle(filehandle);
	empty();
	if(filename)
		myfree( filename);
	}
/*
int Memory::reinit(const char *name) {
	endfile();
	 init(name);
	 return 0;
	}
int Memory::reinit(void) {
	return reinit(filename);
	}
	*/
int Memory::empty(void) {
	for(Treel *iter=tree.treecontrol->roottree;iter!=tree.NIL;iter=tree.next(iter))
		databuffree( iter->data.buf);
	tree.empty();
	return 0;
	}

Memory::~Memory() {
	endfile();
	 DESTROYUSELSEEK;
	}


#ifdef USE_WINFILEnietgebruikt
int myftruncate(FILEHANDLE output,OFFTYPE size) {
	if(setfilepos(output,size)!=size) {
		return -1;
		}
	if(!SetEndOfFile(output)) {
		return -1;
		}
	return 0;
	}
#endif

#ifdef USE_WINFILE
#define myftruncate  sparsesize
extern bool mksparse(HANDLE hOutFile) ;
#else
#define mksparse(x)
#endif
FILEHANDLE Memory::reopen() {
	FILEHANDLE output=openreadwrite(filename);
	if(output==MY_INVALID_HANDLE_VALUE)
	   return MY_INVALID_HANDLE_VALUE;
	if(filesize!=diskfilesize) {
		if(filesize>useddiskfilesize) {
			mksparse(output);
			if( FTRUNCATEHANDLE(output,useddiskfilesize)!=0) {
				closefilehandle(output);
				return MY_INVALID_HANDLE_VALUE;
				}
			}
		if( FTRUNCATEHANDLE(output,filesize)!=0) {
			closefilehandle(output);
	   		return MY_INVALID_HANDLE_VALUE;
			}
		}
	return output;
	}





/* Bepaal of file veranderd is na opstarte of laatste keer saven             */
/* Ga alle nodes af en save die nodes.                                       */
/* Nodes delete                                                              */
/*                                                                           */
/* Saven van een node:                                                       */
/* 	- ga naar positie file;                                                  */
/* 	- schrijf gegevens in file                                               */
/* 	- delete gegevens                                                        */
/* Sat Oct  3 00:50:05 1998                                                  */

int Memory::savenode(FILEHANDLE output, Treel *node) {
	DEBUG("Save " BLOCKPRINT "\n",node->data.blocknr);
	OFFTYPE diff=filesize-(node->data.blocknr*BLOCKSIZE);
	if(diff>0) {
		BEDS_SIZE_T size=(diff<BLOCKSIZE)?diff:BLOCKSIZE;
		if(node->data.size<size) {
			fprintf(stderr,"WARNING: savenode/1: not enough in node " BLOCKPRINT " " SIZE_TPRINT "<" SIZE_TPRINT "\n",node->data.blocknr,size,node->data.size);
			size=node->data.size;
			}
		return writeat(output,node->data.buf,size,node->data.blocknr*BLOCKSIZE);
		}
	return 0;
	}

int Memory::touched(void) {
	if(isnewfile())
		return 0;
#ifdef BIOSDISK
if(!infodisk) 
#endif
{
#ifdef _WIN32

BY_HANDLE_FILE_INFORMATION FInfo;

if(!getfileinfo(&FInfo )) {
	return -1;
	}
#define ongelijk(info) FInfo.info!=FileInfo.info
#define oncmp
#define lastw(name) (char *)&(name.ftLastWriteTime)
if(memcmp(lastw(FInfo),lastw(FileInfo),sizeof(FInfo.ftLastWriteTime)))
	return 5;
if(ongelijk(nFileSizeHigh))
	return 3;
if(ongelijk(nFileSizeLow))
	return 3;
if(ongelijk(nFileIndexHigh))
	return 4;
if(ongelijk(nFileIndexLow))
	return 4;

#else
	STATTYPE st;
	if(STAT(filename,&st)<0)
		return -1;
	if(st.st_mtime!=statdata.st_mtime)
		return 5;
	if(st.st_size!=statdata.st_size)
		return 3;
	if(st.st_ino!=statdata.st_ino)
		return 4;
#endif
	}
	return 0;
	}
OFFTYPE setfilepos(FILEHANDLE file,OFFTYPE offset) {
#ifdef USE_WINFILE
LONG hi=offset>>32;
DWORD lo;
lo=SetFilePointer(file,
	    offset&0xFFFFFFFF,	// number of bytes to move file pointer 
		    &hi,	// address of high-order word of distance to move  
		FILE_BEGIN
				   );
if(lo==0xFFFFFFFF&&GetLastError()) {
	return OFFTYPEINVALID;
	}
return (lo|(((OFFTYPE)hi)<<32));
#else
return LSEEK(file,offset,SEEK_SET);
#endif
}

int Memory::saveall(void) {
int errors=0;
#ifdef USE_WINFILE
FILEHANDLE output=GetStdHandle(STD_ERROR_HANDLE); /* So it doesn't point to someone */
#else
FILEHANDLE output=STDERR_FILENO; 
#endif

	if(readonly) {
		return -2;
		}
#ifdef BIOSDISK
if(infodisk) {
	infodisk=diskreopen(infodisk,O_RDWR);
	for(Treel *iter=tree.treecontrol->roottree;iter!=tree.NIL;iter=tree.next(iter)) {
		if(savenode(output,iter)<0) {
			  infodisk=diskreopen(infodisk,O_RDONLY);
				errors=-3;
				return errors;
				}
		databuffree( iter->data.buf);
		}
	tree.empty();
	infodisk=diskreopen(infodisk,O_RDONLY);
	return errors;
	}
else
#endif
{
	closefilehandle(filehandle); /*Because file is changed */ 

if((output=reopen())==MY_INVALID_HANDLE_VALUE) {
//		readonly=1;
#ifdef USE_WINFILE
errors=WINERROR;
	Memory::errors=GetLastError();
#else
		if(errno)
			errors=errno;
		else
			errors=-1;
#endif
		if((filehandle=openreading(filename))==MY_INVALID_HANDLE_VALUE) {
#ifndef USE_WINFILE
			if(errno)
				errors=errno;
			else
				errors=-1;
#else
	Memory::errors=GetLastError();
#endif
			}
		return errors;
		}
/*
#ifdef USE_WINFILE
if(used_dasd_io) {
	  set_dasd_io( filehandle);
	}
#endif
*/
	for(Treel *iter=tree.treecontrol->roottree;iter!=tree.NIL;iter=tree.next(iter)) {
		if(savenode(output,iter)<0) {
			#ifdef USE_WINFILE
				Memory::errors=GetLastError();
			errors=WINERROR;
#else
				if(errno)
						errors=errno;
				else
					 errors=-4;
#endif
				closefilehandle(output);
				if((filehandle=openreading(filename))==MY_INVALID_HANDLE_VALUE) {
#ifdef USE_WINFILE
	Memory::errors=GetLastError();
#else
					if(errno)
						errors=errno;
					else
						errors=-1;
#endif
					}
				return errors;
				}
		databuffree( iter->data.buf);
		}
	closefilehandle(output);
	tree.empty();
	if((filehandle=openreading(filename))==MY_INVALID_HANDLE_VALUE) {
#ifdef USE_WINFILE
	Memory::errors=GetLastError();
errors=WINERROR;
#else
		if(errno)
			errors=errno;
		else
			errors=-1;
#endif
		}
#ifdef USE_WINFILE
//	STAT(filename,&statdata);
getfileinfo(&FileInfo);
#else
	FSTAT(filehandle,&statdata);
#endif

	useddiskfilesize=diskfilesize=filesize;
	return errors;
	}
}
int Memory::zerochange(void) {
	if(filesize!=newfilesize)
		return 1;
	char tmpbuf[newfilesize];
	getpart(0,newfilesize,tmpbuf); /*Niet verbonden aan file, sector grens maakt niet uit*/
	int *ints=(int *) tmpbuf;
	int len=newfilesize*sizeof(char)/sizeof(int);
	for(int i=0;i<len;i++) 
		if(ints[i])
			return 1;
	return 0;
	}
int Memory::changednode(Treel *node) {
	char *tmpbuf=databufalloc(BLOCKSIZE);
	OFFTYPE diff=filesize-((OFFTYPE)node->data.blocknr*BLOCKSIZE);
int ret=0;
	if(diff>0) {
		BEDS_SIZE_T size=(diff<BLOCKSIZE)?diff:BLOCKSIZE;
		if(node->data.size<size) {
			fprintf(stderr,"WARNING: changednode/1: not enough in node " BLOCKPRINT" " SIZE_TPRINT ">" SIZE_TPRINT "\n",node->data.blocknr,size,node->data.size);
			size=node->data.size;
			}
		blockreadat(tmpbuf,size,node->data.blocknr*BLOCKSIZE);
		ret= memcmp(tmpbuf, node->data.buf, size);
		}
databuffree(tmpbuf);
	return ret;
	}

int Memory::changed(void) {
	OFFTYPE disksize;
	if(isnewfile())
		disksize=(OFFTYPE)newfilesize;
	else
		disksize=diskfilesize;

//	DEBUG("diskfilesize=" OFFPRINT " real size=" OFFPRINT " tosavesize=" OFFPRINT "\n",diskfilesize,disksize,filesize);
	if(disksize!=filesize)
		return -1;
	for(Treel *iter=tree.treecontrol->roottree;iter!=tree.NIL;iter=tree.next(iter)) {
		if(changednode(iter)) {
			return -1;
			}
		}
	for(Treel *iter=tree.treecontrol->roottree;iter!=tree.NIL;iter=tree.next(iter)) 
		databuffree( iter->data.buf);
	tree.empty();
	return errors;
	}
OFFTYPE  Memory::putblocks(const BLOCKTYPE een,char *startbuf,const BLOCKTYPE nr){ 	
BLOCKTYPE end=een+nr;
char *buf=startbuf;
for(BLOCKTYPE iter=een;iter<end;iter++) {
	BEDS_SIZE_T ret=putblock(iter,buf);
	buf+=ret;
	if(ret<BLOCKSIZE) {
		if(ret<0)
			 return OFFTYPEINVALID;
		else
			break;
		}
	}
return buf-startbuf;
}

OFFTYPE  Memory::getblocks(const BLOCKTYPE een,char *startbuf,const BLOCKTYPE nr){ 	
BLOCKTYPE end=een+nr;
char *buf=startbuf;
for(BLOCKTYPE iter=een;iter<end;iter++) {
	BEDS_SIZE_T ret=getblock(iter,buf);
	buf+=ret;
	if(ret<BLOCKSIZE) {
		if(ret<0)
			 return OFFTYPEINVALID;
		else
			break;
		}
	}
return buf-startbuf;
}

#if  defined(USE_WINFILE) ||(defined(HAVE_SENDFILE) && defined(SEEK_DATA))
#include "sparsecp.h"
int Memory::saveto(FILEHANDLE handle) {
	if(useddiskfilesize<filesize) {
		mksparse(handle);
		if(FTRUNCATEHANDLE(handle,filesize)!=0)
			perror("ftruncate");
		}
	
	if(int res;filehandle!=MY_INVALID_HANDLE_VALUE&&(res=sparsecp(filehandle,0,useddiskfilesize,handle))!=0) {
		if(res!=-1) 
			return -1;
#ifndef USE_WINFILE
		lseek(handle,0, SEEK_SET);
#endif
		if(nonsparsecp(filehandle,0,useddiskfilesize,handle)<0) {
		/*	lseek(handle,0, SEEK_SET);
			return nonsparsesaveto(handle);*/
			return -2;
			}
		}
	for(Treel *iter=tree.treecontrol->roottree;iter!=tree.NIL;iter=tree.next(iter)) {
		if(savenode(handle,iter)<0) {
				return -3;
				}
		}
	return 0;
	}
int Memory::nonsparsesaveto(FILEHANDLE handle) {
#else
int Memory::saveto(FILEHANDLE handle) {
#endif
	if(useddiskfilesize<filesize) {
		mksparse(handle);
		if(FTRUNCATEHANDLE(handle,filesize)!=0)
			perror("ftruncate");
		}
	char *buf=databufalloc(BLOCKSIZE);
	BLOCKTYPE nr;
	BEDS_SIZE_T did, ret=0;
	BLOCKTYPE endblock=(useddiskfilesize+BLOCKSIZE-1)/BLOCKSIZE;
	for(nr=0;nr<endblock;nr++) {
			did=getblock(nr, buf);
			ret=writetofile(handle,buf,did);
			if(ret!=did) {
				databuffree(buf);
				return -1;
				}
			}
	databuffree(buf);
	if(diskfilesize>=filesize) {
		if((maxnum((OFFTYPE)(nr-1)*BLOCKSIZE,0)+ret)!=filesize) return -1;
		return 0;
		}
	for(Treel *iter=tree.treecontrol->roottree;iter!=tree.NIL;iter=tree.next(iter)) {
		BLOCKTYPE bl=iter->data.blocknr;
		if(bl>=endblock) {
			OFFTYPE offset=bl*BLOCKSIZE;
			int len=((offset+BLOCKSIZE)>filesize) ?(filesize-offset): iter->data.size;
			writeat(handle,iter->data.buf,len,offset);
			}
		}
		/*
	for(OFFTYPE offset=nr*BLOCKSIZE;offset<filesize;nr++,offset+=BLOCKSIZE) {
		Treel *el=tree.search(nr);
		if(el!=tree.NIL) {
			int len=((offset+BLOCKSIZE)>filesize) ?(filesize-offset): el->data.size;
			writeat(handle,el->data.buf,len,offset);
			}
		} */
//	if((maxnum((OFFTYPE)(nr-1)*BLOCKSIZE,0)+ret)!=filesize) return -1;
	return 0;
	}
BEDS_SIZE_T Memory::getblockpiece(OFFTYPE beg,BEDS_SIZE_T len,char *get) {
	BLOCKTYPE onder=beg/BLOCKSIZE;
	BEDS_SIZE_T ret;
	BEDS_SIZE_T in=beg%BLOCKSIZE;
	Treel *el=tree.search(onder);
	if(el!=tree.NIL) {
		ret=minnum(el->data.size,len);
		memmove(get,el->data.buf+in,ret);
		}
	else {
		ret=readat(get,len,beg);
		}
	return ret;
	}

#ifdef _WIN32
OFFTYPE Memory::getpartaligned(const OFFTYPE beg,const OFFTYPE len,char *const gegs) {
BLOCKTYPE start;
BEDS_SIZE_T begover=beg%BLOCKSIZE;
int blks=(len+begover-1)/BLOCKSIZE+1;	
char *tmpbuf= (char *)alignedalloc(BLOCKSIZE, blks*BLOCKSIZE);
start=(beg/BLOCKSIZE);
BLOCKTYPE end=start+blks;
char *ptr=tmpbuf;
for(BLOCKTYPE nr=start;nr<end;nr++,ptr+=BLOCKSIZE) 
	getblock(nr, ptr);
memcpy(gegs,tmpbuf+begover,len);
alignedfree(tmpbuf);
return len;
}
#endif
OFFTYPE Memory::getpart(OFFTYPE beg,OFFTYPE len,char *gegs) {
	if(beg>=filesize) {
		if(beg==filesize&&!len)
			return 0;
		else
			return OFFTYPEINVALID;
		}
#if defined(_WIN32) 
#ifdef NDEBUG
	if(blockreadonly>1) 
#endif
		return getpartaligned(beg, len,gegs) ;
#endif

	BLOCKTYPE start,bl;
	BEDS_SIZE_T over; 
	OFFTYPE full,end=minnum(beg+len,filesize);
	len=end-beg;
	over=(BLOCKSIZE-(beg%BLOCKSIZE));
	over=minnum(len,over);

	if(getblockpiece(beg,over,gegs)!=over)
		return OFFTYPEINVALID;


	full=len-over;
	if(full>0) {
		char *ptr;
		BEDS_SIZE_T nog; 
		start=(beg/BLOCKSIZE)+1;
		bl=full/BLOCKSIZE+start;
		nog=full%BLOCKSIZE;
		ptr=gegs+over;
		for(BLOCKTYPE nr=start;nr<bl;nr++,ptr+=BLOCKSIZE) {
			if((BLOCKSIZE!=getblock(nr, ptr)))
				return OFFTYPEINVALID;
			}
		if(getblockpiece(end-nog,nog,ptr)!=nog)
			return OFFTYPEINVALID;
		}
	return len;
	}

OFFTYPE Memory::putpart(OFFTYPE beg,OFFTYPE aanlen,char *gegs) {
	if(beg>=filesize) {
		if(beg==filesize&&!aanlen)
			return 0;
		else
			return OFFTYPEINVALID;
		}
	OFFTYPE len=aanlen;
	BLOCKTYPE start,bl;
	BEDS_SIZE_T over;
	OFFTYPE end=minnum(beg+len,filesize),full;
	len=end-beg;
	over=(BLOCKSIZE-(beg%BLOCKSIZE));
	over=minnum(len,over);
	if(over)
		if(putblockpiece(beg,over,gegs)!=over)
			return OFFTYPEINVALID;


	full=len-over;
	if(full>0) {
		char *ptr;
		BEDS_SIZE_T nog;
		start=(beg/BLOCKSIZE)+1;
		bl=full/BLOCKSIZE+start;
		nog=full%BLOCKSIZE;
		ptr=gegs+over;
		for(BLOCKTYPE nr=start;nr<bl;nr++,ptr+=BLOCKSIZE) {
			if((BLOCKSIZE!=putblock(nr, ptr)))
				return OFFTYPEINVALID;
			}
		if(putblockpiece(end-nog,nog,ptr)!=nog)
			return OFFTYPEINVALID;
		}
	return len;
	}


#if  defined(USE_WINFILE) ||(defined(HAVE_SENDFILE) && defined(SEEK_DATA))
BEDS_SIZE_T Memory::saveto(FILEHANDLE handle,OFFTYPE beg,OFFTYPE len) {
	mksparse(handle);
	if(FTRUNCATEHANDLE(handle,len)!=0) {
		perror("ftruncate");
		}
	if(filehandle!=MY_INVALID_HANDLE_VALUE) {
		OFFTYPE left=useddiskfilesize-beg;
		OFFTYPE disklen=minnum(left,len);
		if(int res=sparsecp(filehandle,beg,disklen,handle);res!=0) {
			if(res!=-1)
				return -1;
		#ifndef USE_WINFILE
			lseek(handle,0, SEEK_SET);
		#endif
			if(nonsparsecp(filehandle,beg,disklen,handle)<0) {
				/*	lseek(handle,0, SEEK_SET);
					return nonsparsesaveto(handle,beg, len) ; */
					return -2;
					}
			}
		}
	OFFTYPE end=beg+len;
	OFFTYPE startblock=beg/BLOCKSIZE;
	OFFTYPE endblock=(end+BLOCKSIZE-1)/BLOCKSIZE;
	for(Treel *node=tree.treecontrol->roottree;node!=tree.NIL;node=tree.next(node)) {
		auto blnr=node->data.blocknr;
		if(blnr>=startblock&&blnr<endblock) {
			OFFTYPE off=blnr*BLOCKSIZE;
			OFFTYPE diff=end-off;
			if(diff>0) {
				int bufoff=0;
				if(off<beg) {
					bufoff=beg-off;
					off=beg;
					}
				BEDS_SIZE_T size=(diff<BLOCKSIZE)?diff:BLOCKSIZE;
				writeat(handle,node->data.buf+bufoff,size-bufoff,off-beg);
				}
			}
		}
	return 0;
	}
BEDS_SIZE_T Memory::nonsparsesaveto(FILEHANDLE handle,OFFTYPE beg,OFFTYPE len) {
#else
BEDS_SIZE_T Memory::saveto(FILEHANDLE handle,OFFTYPE beg,OFFTYPE len) {
#endif
	if(beg>=filesize) {
		if(beg==filesize&&!len)
			return 0;
		else
			return -1;
		}
	OFFTYPE full;
	BEDS_SIZE_T over;
//	char buf[BLOCKSIZE];
	sectorbuf buf(BLOCKSIZE, BLOCKSIZE);
//	OFFTYPE end=beg+len;
	int inblock=(beg%BLOCKSIZE);
	over=(BLOCKSIZE-inblock);
	over=minnum(len,over);
	BLOCKTYPE start=(beg/BLOCKSIZE);
	if(over) {
		getblock(start, buf.data());
		if(writetofile(handle,buf.data()+inblock,over)!=over)
			return -1;
		}
	start++;

	full=len-over;
	if(full) {
		BEDS_SIZE_T nog;
		BLOCKTYPE bl=full/BLOCKSIZE+start,nr;
		nog=full%BLOCKSIZE;
		for(nr=start;nr<bl;nr++) {
			if((BLOCKSIZE!=getblock(nr, buf.data())))
				return -1;
			BEDS_SIZE_T ret=writetofile(handle,buf.data(),BLOCKSIZE);
			if(ret!=BLOCKSIZE) {
				return -1;
				}
			}
		if(nog) {
			getblock(nr, buf.data());
			if(writetofile(handle,buf.data(),nog)!=nog)
				return -1;
			}
		}
	return 0;
	}
Treel *Memory::gettreel(BLOCKTYPE blockiter) {
		Treel *el=tree.search(blockiter);
		if(el==tree.NIL) {
			char *gegs=databufalloc(BLOCKSIZE);
			tree.insert((Data){blockiter,gegs,0});
			el=tree.search(blockiter);
			}
		return el;
		}
#ifdef USE_WINFILE

#ifdef USE_PREAD

static inline BEDS_SIZE_T mypread64(FILEHANDLE input,char *buf,BEDS_SIZE_T nr,OFFTYPE offset) {

DWORD did;
OVERLAPPED overlap;
memset(&overlap,'\0',sizeof(overlap));
overlap.Offset = ((unsigned long long)offset)&0xFFFFFFFF;
overlap.OffsetHigh = ((unsigned long long)offset)>>32;
DEBUGGING(OFFPRINT " " SIZE_TPRINT " %llx-%llx\n",offset,nr,(unsigned long long)buf,(unsigned long long)buf+nr);
if(!ReadFile(
    input,	// handle of file to write to 
    buf,	// address of data to write to file 
    nr,	// number of bytes to write 
    &did,	// address of number of bytes written 
    &overlap 	// addr. of structure needed for overlapped I/O  
   )) {
#ifndef NDEBUG
	 DWORD dwerror = GetLastError();
	char errorbuf[150];
	winerror(errorbuf,150,dwerror);
	fprintf(stderr,"ReadFile " OFFPRINT "," SIZE_TPRINT ": %s\n",offset,nr,errorbuf);
#endif
	return -1;
	}
return did;
}


BEDS_SIZE_T mypwrite64(FILEHANDLE outputhandle,const char *buf,BEDS_SIZE_T nr,OFFTYPE offset) {
DWORD did;
OVERLAPPED overlap;
memset(&overlap,'\0',sizeof(overlap));
overlap.Offset = ((unsigned long long)offset)&0xFFFFFFFF;
overlap.OffsetHigh = ((unsigned long long)offset)>>32;

if(!WriteFile(
    outputhandle,	// handle of file to write to 
    buf,	// address of data to write to file 
    nr,	// number of bytes to write 
    &did,	// address of number of bytes written 
    &overlap 	// addr. of structure needed for overlapped I/O  
   ))
	return -1;
else 
	return did;
}
#endif
#endif
BEDS_SIZE_T readfromfile(FILEHANDLE input,char *buf,BEDS_SIZE_T nr) {
#ifdef USE_WINFILE
DWORD did;
if(!ReadFile(

    input,	// handle of file to write to 
    buf,	// address of data to write to file 
    nr,	// number of bytes to write 
    &did,	// address of number of bytes written 
    NULL 	// addr. of structure needed for overlapped I/O  
   ))
	return -1;
else 
	return did;
#else
return LREAD(input,buf,nr);
#endif
}
BEDS_SIZE_T writetofile(FILEHANDLE output,const char *buf,BEDS_SIZE_T nr) {
#ifdef USE_WINFILE
DWORD did;
if(!WriteFile(

    output,	// handle of file to write to 
    buf,	// address of data to write to file 
    nr,	// number of bytes to write 
    &did,	// address of number of bytes written 
    NULL 	// addr. of structure needed for overlapped I/O  
   ))
	return -1;
else 
	return did;
#else
return LWRITE(output,buf,nr);
#endif
}

#ifdef USE_PREAD
#define USE_PWRITE 1
#endif

BEDS_SIZE_T Memory::writeat(FILEHANDLE output,char *ptr,BEDS_SIZE_T size,OFFTYPE offset) {

#ifdef NEEDS_BLOCKMODE
	assert(((blockreadonly>1)?(((intptr_t)ptr)%BLOCKSIZE==0):true));
#endif
#ifdef BIOSDISK
	if(infodisk)	{
		BEDS_SIZE_T count=size/blockreadonly;
		if(count<=0) {
		  fprintf(stderr,"Memory::writeat " SIZE_TPRINT " is not a multiple of %d ( disk blocksize)\n",size,blockreadonly);
		return -5;
			}
		if(diskwrite(infodisk, ptr,size/blockreadonly,offset/blockreadonly)) {
			return size;
			}
		return -4;
		}
	else	
#endif
{
#ifdef USE_PWRITE
	BEDS_SIZE_T ret;
	ret=LPWRITE(output,ptr,size,offset);
	if(ret!=size) {
		fprintf(stderr,"write failed " SIZE_TPRINT "!=" SIZE_TPRINT "\n",ret,size);
		return -2;
		}
	return ret;
#else
	STARTUSELSEEK;
	OFFTYPE isof=setfilepos(output,offset);
	BEDS_SIZE_T ret;
	if(isof!=offset) {
		fprintf(stderr,"writeat: lseek failed " OFFPRINT "!=" OFFPRINT "\n",isof,offset);
		STOPUSELSEEK;
		return -1;
		}
	ret=writetofile(output,ptr,size);
	if(ret!=size) {
		fprintf(stderr,"write failed " SIZE_TPRINT "!=" SIZE_TPRINT "\n",ret,size);
		STOPUSELSEEK;
		return -2;
		}
	STOPUSELSEEK;
	return ret;
#endif
 }
	}
BEDS_SIZE_T Memory::blockreadat(char *ptr,BEDS_SIZE_T size,OFFTYPE offset) {

if(offset>=useddiskfilesize) {
#ifdef HAVE_BZERO
	bzero(ptr,size);
#else
	memset(ptr,'\0',size);
#endif
	return size;
	}
return realblockreadat(ptr,size,offset);
}
BEDS_SIZE_T Memory::realblockreadat(char *ptr,BEDS_SIZE_T size,OFFTYPE offset) {

#ifdef NEEDS_BLOCKMODE
	assert(((blockreadonly>1)?(((intptr_t)ptr)%BLOCKSIZE==0):true));
#endif
		BEDS_SIZE_T ret;
#ifdef BIOSDISK
	if(infodisk)	{
		BEDS_SIZE_T count=size/blockreadonly;
		if(count<=0) {
		  fprintf(stderr,"Memory::blockreadat " SIZE_TPRINT " is not a multiple of %d ( disk blocksize)\n",size,blockreadonly);
                        return -5; 
			}
		if(diskread(infodisk, ptr,count,offset/blockreadonly)) {
			return size;
			}
		return -4;
		}
	else	
#endif
{
#ifdef USE_PREAD
		ret=LPREAD(filehandle,ptr,size,offset);
/*
		if(ret!=size) {
//			fprintf(stderr,"readat failed " SIZE_TPRINT  "!=" SIZE_TPRINT "\n",ret,size);
			return -2;
			}
*/
		return ret;
#else
		STARTUSELSEEK;
		OFFTYPE isof=setfilepos(filehandle,offset);
		if(isof!=offset) {
//			fprintf(stderr,"readat: lseek failed " OFFPRINT "!=" OFFPRINT "\n",isof,offset);
			STOPUSELSEEK;
			return -1;
			}
		ret= readfromfile(filehandle,ptr,size);
/*
		if(ret!=size) {
//			fprintf(stderr,"readat failed " SIZE_TPRINT  "!=" SIZE_TPRINT "\n",ret,size);
			STOPUSELSEEK;
			return -2;
			}
*/
		STOPUSELSEEK;
		return ret;
#endif
}
}

#ifdef NEEDS_BLOCKMODE
inline BEDS_SIZE_T Memory::readat(char *ptr,BEDS_SIZE_T size,OFFTYPE offset) {
	if(blockreadonly>1) {
		OFFTYPE offunder=(offset/blockreadonly)*blockreadonly;
		BEDS_SIZE_T extra=offset-offunder;
		BEDS_SIZE_T blen=(((size+extra)+blockreadonly-1)/blockreadonly)*blockreadonly;
if(!extra&&blen==size)
	goto DIRECT;
char *buf=databufalloc(blen);
fprintf(stderr,"readat: %llx-%llx ",(unsigned long long)buf,(unsigned long long) buf+blen);
		if(blockreadat(buf,blen,offunder)<(size+extra))
				return -1;
		memcpy(ptr,buf+extra,size);
		databuffree(buf);
		return size;		
		}
	else {
DIRECT:
		return blockreadat(	ptr,size,offset);
		}
	}
#else
inline BEDS_SIZE_T Memory::readat(char *ptr,BEDS_SIZE_T size,OFFTYPE offset) {
		return blockreadat(	ptr,size,offset);
	}
#endif
BEDS_SIZE_T Memory::getblock(BLOCKTYPE blocknr,char *buf) {

	OFFTYPE offset=(blocknr*BLOCKSIZE);
	if(offset>=filesize)
		return 0;
	Treel *el=tree.search(blocknr);
	if(el!=tree.NIL) {
		memmove(buf,el->data.buf,el->data.size);
		return minnum(filesize-offset,el->data.size);
		}
	else {
		int len=minnum(filesize-offset,BLOCKSIZE);
		return blockreadat(buf,len,offset);
		}
	}

BEDS_SIZE_T Memory::putblock(BLOCKTYPE blocknr,char *buf) {
	OFFTYPE offset=(blocknr*BLOCKSIZE);
	if(offset>=filesize)
		return 0;
	BEDS_SIZE_T thissize=minnum(filesize-offset,BLOCKSIZE);
	Treel *el=gettreel(blocknr);
	memmove(el->data.buf,buf,thissize);
	el->data.size=thissize;
	return thissize;
	}

void	Memory::setborder(OFFTYPE pos) {
	BLOCKTYPE under=pos/BLOCKSIZE;
	Treel *el=tree.search(under);
	if(el==tree.NIL)
		return;
	OFFTYPE offset=under*BLOCKSIZE;
	el->data.size=minnum(filesize-offset,BLOCKSIZE);
	}
void Memory::extend(OFFTYPE newsize) {
	OFFTYPE oldsize=filesize;
	BLOCKTYPE under=oldsize/BLOCKSIZE;
	Treel *el=tree.search(under);
	if(el==tree.NIL) {
		char buf[BLOCKSIZE]; /*Alignment doesn't matter, you can't extend \\.\PhysicalDriveX */
		OFFTYPE begin=under*BLOCKSIZE,sizebuf=filesize-begin;
		blockreadat(buf,sizebuf,begin);
		putpart(begin,sizebuf,buf) ;
		el=tree.search(under);
		}
/*	else {
		long begin=(oldsize%BLOCKSIZE);
		memset(el->data.buf+begin,'\0',BLOCKSIZE-begin);
		} */
	filesize=newsize;
	OFFTYPE offset=under*BLOCKSIZE;
	el->data.size=minnum(filesize-offset,BLOCKSIZE);
	return;
	}

void Memory::undotruncate(OFFTYPE newsize,OFFTYPE useddisk) {
//void Memory::undotruncate(OFFTYPE newsize) {
/*	OFFTYPE wassize=filesize;
	BLOCKTYPE  bl=filesize/BLOCKSIZE;
	char buf[BLOCKSIZE];
	getblock(bl,buf); */
	extend(newsize) ;
	/*
	if(newsize<=diskfilesize) {
		useddiskfilesize=newsize;
		}
	else {
		fprintf(stderr,"Undo truncate to larger file " OFFPRINT " > " OFFPRINT "\n",newsize,diskfilesize);
		}
		*/
	}
void Memory::truncatefile(OFFTYPE pos) {			/* v2 */
	Treel *el=tree.search(filesize);
	if(el!=tree.NIL) {		/* CAUTION old data is not removed. No problem: extending goes always with putpart? (extend, undo, saveall?) */

		el->data.size=BLOCKSIZE;
		}
	filesize=pos;
	useddiskfilesize=minnum(useddiskfilesize,filesize);
	setborder(pos);
	}
BEDS_SIZE_T Memory::putblockpiece(OFFTYPE  beg,BEDS_SIZE_T len,char *get) {
	 BLOCKTYPE onder=beg/BLOCKSIZE;
	BEDS_SIZE_T ret;
	 OFFTYPE bstart=onder*BLOCKSIZE;
	BEDS_SIZE_T in=beg%BLOCKSIZE;
	Treel *el=tree.search(onder);
	BEDS_SIZE_T thissize=minnum(filesize-bstart,BLOCKSIZE);
	if(el!=tree.NIL) {
		if(el->data.size<thissize) {
			fprintf(stderr,"putblockpiece " SIZE_TPRINT " (intree)!=" SIZE_TPRINT " (should)\n",el->data.size,thissize);
			char *gegs=databufalloc(BLOCKSIZE);
			char *old; 
			ret=blockreadat(gegs,thissize,bstart);
			if(ret!=thissize) {
				fprintf(stderr,"putblockpiece " SIZE_TPRINT "(read)!=" SIZE_TPRINT "(should)\n",ret,thissize);
				}
			memmove(gegs,el->data.buf,ret);
			old=el->data.buf;
			el->data.buf=gegs;
			databuffree( old);
			el->data.size=ret;
			}
		memmove(el->data.buf+in,get,len);
		}
	else {
		char *gegs=databufalloc(BLOCKSIZE); 
		if(in!=0||len!=thissize)
			ret=blockreadat(gegs,thissize,bstart);
		/*
		if(ret!=thissize) {
			fprintf(stderr,"2:putblockpiece %ld(read)!=%ld(should)\n",ret,thissize);
			}*/
		memmove(gegs+in,get,len);
		tree.insert((Data){onder,gegs,thissize});
		}
	return len;
	}

/*
OFFTYPE Memory::search(OFFTYPE beg, signed char *str,int len) {
#ifdef DONTINTCURSES 
DECPDDEST();
#endif
	OFFTYPE ret= OFFTYPEINVALID;
		switch(lastforward&0x7) {
								//forward, icase, norm
		case 3: ret= casesearch(beg,str,len);break;
		case 1: ret= icasesearch(beg,str,len);break;
		case 7:ret= regexsearch(beg,str,len);break;
		case 5: ret= icaseregexsearch(beg,str,len);break;



		case 2: ret= backsearch(beg,str,len);break;
		case 0: ret= backicasesearch(beg,str,len);break;
		case 6: ret= backregexsearch(beg,str,len);break;
		case 4: ret= backicaseregexsearch(beg,str,len);break;
		default: ret= OFFTYPEINVALID;break;
		};
#ifdef DONTINTCURSES 
INCPDDEST();
#endif
	return ret;
	}

*/

#ifdef TEST
//#include "basic.h"
int main(int argc, char **argv) {
#define MAXBUF 0xffff
	int pos,len;
	char buf[MAXBUF];
	char command[256]="cat   /tmp/memoryXXXXXX",*hierfile=command+6;
	if(argc!=2) {
		fprintf(stderr,"Gebruik: %s filename\n",argv[0]);
		exit(1);
		}
	Memory mem(argv[1]);

	if(mem.error())	{
		fprintf(stderr,"%s: error \n",argv[0]);
		exit(2);
		}
	printf("size=%d\n",mem.size());
	puts("Give filepostition and lenght (pos len)");
	while(gets(buf)) {
		if(buf[0]=='q')
			break;
		if((sscanf(buf,"%d %d",&pos,&len)<2)||len>MAXBUF)
			continue;
		int handle,newlen;
		newlen=mem.getpart(pos,len,buf);
		strcpyn(hierfile+11,"XXXXXX",239);
		if(!(mktemp(hierfile)))
			fatal("tmpnam(/tmp/memory) fails\n");
		puts(hierfile);
		if((handle=creat(hierfile,0600))<0)
			fatal("creat(%s)\n",hierfile);
		write(handle,buf,newlen);
		LCLOSE(handle);
		system(command);
		if((handle=LOPEN(hierfile,O_RDONLY BINARY_FLAG))<0)
			fatal("Can't open %s for readonly\n",hierfile);
		seekread(handle,0L,SEEK_SET,buf,newlen);
		LCLOSE(handle);
		len=mem.putpart(pos,len,buf);
		puts("Give filepostition and lenght (pos len)");
		}
	mem.tree.showtree();
	puts("Save file (j/n)");
	gets(buf);
	if(buf[0]=='j') {
		mem.saveall();
		puts("Saved");
		}
	mem.tree.showtree();
	return 0;
	}
#endif
#ifdef TESTMEM
extern int testmem(void) ;
int testmem(void) {
const char *filename="\\\\.\\PhysicalDrive0";

	Memory mem(filename);

	if(mem.error())	{
		fprintf(stderr,"%s: error \n",filename);
		exit(2);
		}
OFFTYPE pos=0;
long blk=512,len=blk;
char buf[len];
for(int i=0;i<9;i++,pos+=blk) {
		fprintf(stderr,"t: ");
		int newlen=mem.getpart(pos,len,buf);

		}
	return 0;
}
	
#endif
