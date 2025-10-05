/*
win16 interface to bios.
Compiled by the win16 compiler of borland 4.5
According to Microsoft Win95 was designed so for raw disk access "Win32 applications must thunk to a Win16 DLL and have that DLL call the DOS Protected Mode Interface (DPMI) Simulate Real Mode Interrupt function to call Int 13h BIOS disk functions on hard disks" (http://support.microsoft.com/support/kb/articles/q137/1/76.asp).
It is not needed for windows NT/2000/xp

Most of the procedures are a modified version of some procedures found
in the file diskio2.cpp in the source of explore2fs 
(http://uranus.it.swin.edu.au/~jn/linux/)

Fri May 24 21:27:32  2002
Jaap Korthals Altes <binaryeditor@gmx.com>
*/
#include <windows.h>
#include <stdio.h>
#include <mem.h>
#include <stdarg.h>
#include <stdlib.h>
#include "biosthun.h"
#include "biosdisk.h"

typedef struct 
{
	BYTE size;
	BYTE reserved;
	WORD count;
	DWORD buffer;
	DWORD startLo;
	DWORD startHi;
} DiskAddressPacket;



typedef struct 
{
    WORD size;
    WORD flags;
    DWORD cylinders;
    DWORD heads;
    DWORD sectors;
    DWORD sectorsLo;
    DWORD sectorsHi;
    WORD bytesPerSector;
//---v2.0+ ---
    DWORD EDDptr;
//---v3.0 ---
    WORD signature;
    BYTE v3size;
    BYTE reserved[3];
    BYTE bus[4];
    BYTE theinterface[8];
    BYTE interfacePath[8];
    BYTE devicePath[8];
    BYTE reserved2;
    BYTE checksum;
}DriveParameters;

//--------------------------------------------------------------------
	// Code in the 16-bit DLL

	// Converts two BYTEs into a WORD.  This is useful for working with
	// a RMCS, but was not provided in WINDOWS.H.


#ifndef MAKEWORD
	#define MAKEWORD(low, high) \
				  ((WORD)((((WORD)(high)) << 8) | ((BYTE)(low))))
#endif

	#define SECTOR_SIZE 512       // Size, in bytes, of a disk sector
	#define CARRY_FLAG  0x0001

#ifndef WIN32
	typedef BYTE FAR *LPBYTE;
#endif

	typedef struct tagRMCS {

		DWORD edi, esi, ebp, RESERVED, ebx, edx, ecx, eax;
		WORD  wFlags, es, ds, fs, gs, ip, cs, sp, ss;

	} RMCS, FAR* LPRMCS;

	BOOL FAR PASCAL SimulateRM_Int (BYTE bIntNum, LPRMCS lpCallStruct);

	void FAR PASCAL BuildRMCS (LPRMCS lpCallStruct);

#ifdef DODEBUG
#define error DEBUG
void  DEBUG(char *msg, ...)
{
	va_list ap;
   FILE *fp;
	va_start(ap, msg);


fp=fopen("c:\\tmp\\debug.out","a");
if(!fp)
	return;
vfprintf(fp,msg,ap);
fclose(fp);
   va_end(ap);
}
#else
#define  DEBUG 
#endif
#ifdef DODEBUG
void BUGWRITE(char *buf,int len)
{
	FILE *fp;


fp=fopen("c:\\tmp\\debug.out","ab");
if(!fp)
	return;
fwrite(buf,len,1,fp);
fclose(fp);
}
#endif

DWORD FAR PASCAL _export dosalloc(DWORD size) {
		return GlobalDosAlloc (size);
		}
UINT FAR PASCAL _export dosfree(UINT sel) {
  return GlobalDosFree(sel);
	}

BOOL _export FAR PASCAL diskwrite(DISKIDENT disk,void FAR *buf,BLOCKCOUNT count,DWORD hi,DWORD low )  
{
BOOL   fResult;
RMCS   callStruct;
DWORD  dapBuffer;     // Return value of GlobalDosAlloc().
LPBYTE RMlpdapBuffer;    // Real-mode buffer pointer
LPBYTE PMlpdapBuffer;    // Protected-mode buffer pointer
DiskAddressPacket FAR *lpdap;
dapBuffer = GlobalDosAlloc(sizeof(DiskAddressPacket)); 
if(!dapBuffer) return FALSE;
RMlpdapBuffer = (LPBYTE)MAKELONG(0, HIWORD(dapBuffer));
PMlpdapBuffer = (LPBYTE)MAKELONG(0, LOWORD(dapBuffer));
lpdap = (DiskAddressPacket FAR *)PMlpdapBuffer;
lpdap->size   = sizeof(DiskAddressPacket);
lpdap->count  = count;
lpdap->buffer = (DWORD)buf;
lpdap->startLo  = low;
lpdap->startHi  = hi;
BuildRMCS (&callStruct);
callStruct.eax = MAKEWORD(0, 0x43);        // BIOS read
callStruct.edx = MAKEWORD(disk, 0);       // Drive #
callStruct.ds  = HIWORD(RMlpdapBuffer);
callStruct.esi  = LOWORD(RMlpdapBuffer);
fResult = SimulateRM_Int (0x13, &callStruct);
if(fResult) // DPMI success
	if(callStruct.wFlags & CARRY_FLAG) // check carry flag for error
		fResult = FALSE;
/*
if(!fResult) {
	// DEBUG("Failed\n");
	}
else {
	// DEBUG("Success\n");
	}
*/
GlobalDosFree (LOWORD(dapBuffer));
return fResult;
}

static int resetdisk(DISKIDENT disk) 
{
	int ah;
	RMCS   callStruct;
	BuildRMCS (&callStruct);
	callStruct.eax =(DWORD)0L;
	callStruct.edx =(DWORD)disk;
	if(SimulateRM_Int (0x13, &callStruct)) 
	{
		ah=HIBYTE(LOWORD(callStruct.eax));

#ifdef DODEBUG
		if(ah) {
				error("reset failed with %d\n",ah);
				}
#endif
		return ah;
		}	
	return 0x100;
}

WORD _export FAR PASCAL diskread(DISKIDENT disk,void FAR *buf,BLOCKCOUNT count,DWORD hi,DWORD low )  
{
	WORD retcount=0;
	RMCS   callStruct;
	DWORD  dapBuffer;     // Return value of GlobalDosAlloc().
	LPBYTE RMlpdapBuffer;    // Real-mode buffer pointer
	LPBYTE PMlpdapBuffer;    // Protected-mode buffer pointer
	DiskAddressPacket FAR *lpdap;
	dapBuffer = GlobalDosAlloc(sizeof(DiskAddressPacket)); 
	if(!dapBuffer) return FALSE;
	RMlpdapBuffer = (LPBYTE)MAKELONG(0, HIWORD(dapBuffer));
	PMlpdapBuffer = (LPBYTE)MAKELONG(0, LOWORD(dapBuffer));
	lpdap = (DiskAddressPacket FAR *)PMlpdapBuffer;
	{
		int i,times;
		if(disk&0x80) /*Harddisk */
			times=1;
		else
			times=3;
		i=0;
		do 
		{
#ifdef DODEBUG
			error("%lx:%lx diskread %d\n",hi,low,i);
#endif
			lpdap->size   = sizeof(DiskAddressPacket);
			lpdap->count  = count;
			lpdap->buffer = (DWORD)buf;
			lpdap->startLo  = low;
			lpdap->startHi  = hi;
			BuildRMCS (&callStruct);
			callStruct.eax = MAKEWORD(0, 0x42);        // BIOS read
			callStruct.edx = MAKEWORD(disk, 0);       // Drive #
			callStruct.ds  = HIWORD(RMlpdapBuffer);
			callStruct.esi  = LOWORD(RMlpdapBuffer);
				if(SimulateRM_Int (0x13, &callStruct)) // DPMI success
				{
					if(!(callStruct.wFlags & CARRY_FLAG)) // check carry flag for error
					{
						retcount=lpdap->count;
						break;
					}
				}
			i++;
		}  while(i<times&&!resetdisk(disk));
	}
	GlobalDosFree (LOWORD(dapBuffer));
	return retcount;
}

	/*--------------------------------------------------------------------
	  BuildRMCS()

	  Initializes a real mode call structure to contain zeros in all its
	  members.

	  Parameters:
		  lpCallStruct
			  Points to a real mode call structure

	  Comments:
		  lpCallStruct is a protected-mode selector:offset address, not a
		  real-mode segment:offset address.

	--------------------------------------------------------------------*/

	void FAR PASCAL BuildRMCS (LPRMCS lpCallStruct)
		{
		_fmemset (lpCallStruct, 0, sizeof (RMCS));
		}


	/*--------------------------------------------------------------------
	  SimulateRM_Int()

	  Allows protected mode software to execute real mode interrupts such
	  as calls to DOS TSRs, DOS device drivers, etc.

	  This function implements the "Simulate Real Mode Interrupt" function
	  of the DPMI specification v0.9.

	  Parameters
		  bIntNum
			  Number of the interrupt to simulate

		  lpCallStruct
			  Call structure that contains params (register values) for
			  bIntNum.

	  Return Value
		  SimulateRM_Int returns TRUE if it succeeded or FALSE if it
		  failed.

	  Comments
		  lpCallStruct is a protected-mode selector:offset address, not a
		  real-mode segment:offset address.

	--------------------------------------------------------------------*/

	BOOL FAR PASCAL SimulateRM_Int (BYTE bIntNum, LPRMCS lpCallStruct)

		{
		BOOL fRetVal = FALSE;      // Assume failure

		_asm {
				push di

				mov  ax, 0300h         //; DPMI Simulate Real Mode Int
				mov  bl, bIntNum       //; Number of the interrupt to simulate
				mov  bh, 01h           //; Bit 0 = 1; all other bits must be 0
				xor  cx, cx            //; No words to copy
				les  di, lpCallStruct
				int  31h                   //; Call DPMI
				jc   END1                  //; CF set if error occurred
				mov  fRetVal, TRUE
			END1:
				pop di
			  }
		return (fRetVal);
		}

DWORD FAR PASCAL __export GetDriveParameters(struct diskinfo FAR *infodisk)
{
if(resetdisk(infodisk->drive))
		return FALSE;	
else
{
	BOOL   fResult;
	RMCS   callStruct;

	DWORD  dpBuffer;     // Return value of GlobalDosAlloc().
	LPBYTE RMlpdpBuffer;    // Real-mode buffer pointer
	LPBYTE PMlpdpBuffer;    // Protected-mode buffer pointer

	DriveParameters far *dpptr;
	// DEBUG("drive=%hd\n",(WORD)infodisk->drive);
	// DEBUG("sizeof(struct diskinfo)=%d\n",sizeof(struct diskinfo));
	BuildRMCS (&callStruct);

	callStruct.eax = MAKEWORD(0, 0x41);        // IBM/MS INT 13 Extensions - INSTALLATION CHECK
	callStruct.ebx = 0x55AA;
	callStruct.edx = MAKEWORD(infodisk->drive, 0);

	fResult = SimulateRM_Int (0x13, &callStruct);
	if(fResult) // DPMI success
	{
		if(callStruct.wFlags & CARRY_FLAG) // check carry flag for error
		{
			fResult = FALSE;
		}
		if(LOWORD(callStruct.ebx) != 0xAA55)
		{
			fResult = FALSE;
		}
	}
	if(fResult)
	{
		// get the version info
		// version = HIBYTE(LOWORD(callStruct.eax));

		// query the specified drive

		dpBuffer = GlobalDosAlloc (sizeof(DriveParameters));
		if (!dpBuffer)
		{
			return FALSE;
		}

		RMlpdpBuffer = (LPBYTE)MAKELONG(0, HIWORD(dpBuffer));
		PMlpdpBuffer = (LPBYTE)MAKELONG(0, LOWORD(dpBuffer));

		dpptr=(DriveParameters far *)PMlpdpBuffer;

		dpptr->size = sizeof(DriveParameters);

		BuildRMCS (&callStruct);

		callStruct.eax = MAKEWORD(0, 0x48); // GET DRIVE PARAMETERS
		callStruct.edx = MAKEWORD(infodisk->drive, 0);

		callStruct.ds  = HIWORD(RMlpdpBuffer);
		callStruct.esi  = LOWORD(RMlpdpBuffer);

		fResult = SimulateRM_Int (0x13, &callStruct);
		if(fResult) // DPMI success
		{
			if(callStruct.wFlags & CARRY_FLAG) // check carry flag for error
			{
				fResult = FALSE;
			}
		}
		if(fResult)
		{
			// copy parameters into supplied structure
		   _fmemcpy(&infodisk->cylinders,&dpptr->cylinders,22);
			// DEBUG("cylinders=%ld\n",dpptr->cylinders);
			// DEBUG("heads=%ld\n",dpptr->heads);
			// DEBUG("sectors=%ld\n",dpptr->sectors);
			// DEBUG("sectorsLo=%ld\n",dpptr->sectorsLo);
			// DEBUG("sectorsHi=%ld\n",dpptr->sectorsHi);

		}
		GlobalDosFree (LOWORD(dpBuffer));
	}
	return fResult;
}
}
