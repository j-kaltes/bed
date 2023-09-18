#include "desturbe.h"
#include "system.h"
#include "editor.h"
#include "main.h"
#include "dialog.h"
#define erroruit printf
#if defined(USE_WINFILE)||defined(__linux__)
#ifdef USE_WINFILE
#include <windows.h>
#ifdef __CYGWIN
#include <w32api/winbase.h>
#include <w32api/winioctl.h>
#include <w32api/winnt.h>
#endif
#include <stdio.h>
#include <winioctl.h>
//#include <WinIoCtl.h>
#include "win.h"
#endif
#ifdef __linux__
#include <sys/stat.h>
#include <sys/sysmacros.h>
#endif

#ifdef _WIN32
inline FILEHANDLE  opennothing(const char *filename) {return CreateFile(filename,  0, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING,0,NULL); }
#else
extern FILEHANDLE openreading(const char *filename) ;
#define opennothing(filename)  openreading(filename) 
#endif

#ifdef USE_WINFILE
extern OSVERSIONINFO WindowsVersion;
long long getdisklength(HANDLE filehandle) {
	DWORD retsize;

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
			))
			return devlen.Length.QuadPart;
		}

	PARTITION_INFORMATION parinfo;
    if(DeviceIoControl(filehandle, IOCTL_DISK_GET_PARTITION_INFO, NULL, 0, &parinfo, sizeof(parinfo), &retsize,NULL)) {
	return (OFFTYPE)parinfo.PartitionLength.QuadPart;
	}

	return (OFFTYPE)-1;
	}
#endif
char *assigndev(int len) {
	char **devptr=(char **)malloc(sizeof(char *)+len);
	*devptr=NULL;
	return (char *)&devptr[1];
	}
inline void freearray(char **devices,int &maxdriv) {
	INCDEST();
	for(int i=0;i<maxdriv;i++) {
		char **devptr= (char **)devices[i];
		if(*--devptr)
			free(*devptr);
		free(devptr);
		}
	free(devices);
	maxdriv=0;
	DECDEST();
	}
#ifdef USE_WINFILE
#ifdef BIOSDISK
#include "biosdisk.h"
#endif
#define MAXDRIVES 200
#ifdef BIOSDISK
bool doswin(void) {
	return WindowsVersion.dwOSVersionInfoSize&&WindowsVersion.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS;
	}

//s/=[^=]*malloc/=assigndev
int getbiosdrives(char ***devicesptr) {
	int drivenr=0; /*total number of 'drives' (physical+logical) */
	char **devices=(char **)malloc(sizeof(char *)* MAXDRIVES );
if(!devices)
	return -1;
	*devicesptr=devices;
	for(drivenr=0;drivenr<MAXDRIVES;drivenr++) {
			if(!(diskpresent(drivenr|0x80))) {
					break;
					}
			const int end=17;
			char disk[]="\\\\.\\PHYSICALDRIVE";
			char *dev=devices[drivenr]=assigndev(end+4+1);	
			strcpy(dev,disk);
			sprintf(dev+end,"%d",drivenr);	
			}
	for(int flopiter=0;drivenr<MAXDRIVES;drivenr++,flopiter++) {
			if(!(diskpresent(flopiter))) {
					break;
					}
			devices[drivenr]=assigndev(7);	
			sprintf(devices[drivenr],"\\\\.\\%c:",'A'+flopiter);
			}
	if(drivenr<1)
		free(devices);
	return drivenr;
	}
#endif

extern char messagebuf[];

typedef char VolNameType;
typedef char* VolNameTypePtr;

typedef WINBASEAPI BOOL WINAPI (*GetVolumeNameForVolumeMountPointAType)(LPCSTR,LPSTR,DWORD);
typedef WINBASEAPI HANDLE WINAPI (*FindFirstVolumeAType)(LPCSTR,DWORD);
typedef WINBASEAPI BOOL WINAPI (*FindNextVolumeAType)(HANDLE,LPCSTR,DWORD);
typedef WINBASEAPI BOOL WINAPI (*FindVolumeCloseType)(HANDLE);
typedef WINBASEAPI BOOL WINAPI (*GetVolumePathNamesForVolumeNameAType)(LPCSTR,LPSTR,DWORD,PDWORD);

//typedef WINBASEAPI BOOL WINAPI (*GetVolumeInformationByHandleWType)(HANDLE,LPSTR,DWORD,PDWORD,PDWORD,PDWORD,LPSTR,DWORD);
//typedef WINBASEAPI BOOL WINAPI (*GetVolumeInformationByHandleWType)(  HANDLE, LPWSTR, DWORD, LPDWORD, LPDWORD, LPDWORD, LPWSTR, DWORD);
class Volumes {
private:
BOOL success;
public:
 GetVolumeNameForVolumeMountPointAType GetVolumeNameForVolumeMountPointA;
 FindFirstVolumeAType FindFirstVolumeA;
 FindNextVolumeAType FindNextVolumeA;
 FindVolumeCloseType FindVolumeClose;
 GetVolumePathNamesForVolumeNameAType GetVolumePathNamesForVolumeNameA;
//GetVolumeInformationByHandleWType GetVolumeInformationByHandleW;
	Volumes(void) { 
		HMODULE kernel32=NULL;
		if(!(kernel32 = GetModuleHandle( "Kernel32.dll"))) {
			erroruit("GetModuleHandle( Kernel32.dll) failed");
			success=FALSE;return;
			}
/*
		if(!(GetVolumeInformationByHandleW =(GetVolumeInformationByHandleWType) GetProcAddress(kernel32,"GetVolumeInformationByHandleW"))) {
			erroruit("GetProcAddress(kernel32,GetVolumeInformationByHandleW) failed");
			}
*/
		if(!(FindFirstVolumeA =(FindFirstVolumeAType) GetProcAddress(kernel32,"FindFirstVolumeA"))) {
			erroruit("GetProcAddress(kernel32,FindFirstVolumeA) failed");
			success=FALSE;return;
			}
		if(!(FindNextVolumeA =(FindNextVolumeAType) GetProcAddress(kernel32,"FindNextVolumeA"))) {
			erroruit("GetProcAddress(kernel32,FindNextVolumeA) failed");
			success=FALSE;return;
			}
		if(!(FindVolumeClose =(FindVolumeCloseType) GetProcAddress(kernel32,"FindVolumeClose"))) {
			erroruit("GetProcAddress(kernel32,FindVolumeClose) failed");
			success=FALSE;return;
			}
		if(!(GetVolumePathNamesForVolumeNameA =(GetVolumePathNamesForVolumeNameAType) GetProcAddress(kernel32,"GetVolumePathNamesForVolumeNameA"))) {
			erroruit("GetProcAddress(kernel32,GetVolumePathNamesForVolumeNameA) failed");
			success=FALSE;return;
			}
		if(!(GetVolumeNameForVolumeMountPointA =(GetVolumeNameForVolumeMountPointAType) GetProcAddress(kernel32,"GetVolumeNameForVolumeMountPointA"))) {
			erroruit("GetProcAddress(kernel32,GetVolumeNameForVolumeMountPointA) failed");
			success=FALSE;return;
			}

	success=TRUE;
	};

inline int getvolpaths(char *volume, int volnamelen,char *buf, int *lastdriveptr) {
	volume[volnamelen]='\\';
	volume[volnamelen+1]='\0';
	DWORD pathslen;
const	int takelen=MAX_PATH-20;
		  /*maybe maxbuf is too small, but not displayable anyway */
	*buf='\0';
       BOOL ret=GetVolumePathNamesForVolumeNameA( volume, buf, takelen, &pathslen); /*Windows XP returns wrong values in pathslen */
	volume[volnamelen]='\0';
	DWORD lasterror=0;	
	if((!ret&&(lasterror=GetLastError())!=ERROR_MORE_DATA)||!*buf) {
		return 0;
		}	
	int drivelen,buflen=0;
	 do {   drivelen=strlen(buf+buflen);
		buflen+=drivelen;
		} while(buflen<(takelen-1)&&buf[buflen+1]&&(buf[buflen++]=' '));
	if(lasterror==ERROR_MORE_DATA) {
		buflen-=(drivelen+1);
		if(buflen<1) {
			return 0;
			}
		buf[buflen]='\0';
		for(int i=buflen-1;i>0;i--)
			if(buf[i]==' ') {
				*lastdriveptr=i+1;	
				return buflen;
				}
		*lastdriveptr=0;
		return buflen;
		}
	*lastdriveptr=buflen	- drivelen;
	return  buflen;
	}
inline int volumenamefs(const char *drive,char *buf,int maxbuf) {
  	DWORD _VolumeSerialNumber, _MaximumComponentLength, _FileSystemFlags;
	DWORD nFileSystemNameSize=40;
	char FileSystemNameBuffer[nFileSystemNameSize];
if(GetVolumeInformationA(drive,buf ,maxbuf ,&_VolumeSerialNumber, &_MaximumComponentLength, &_FileSystemFlags, FileSystemNameBuffer, nFileSystemNameSize)) {
		int buflen=strlen(buf);
		if(buflen)
			*(buf+buflen++)=' ';
		if(const char * const uit=strcpyn(buf+buflen,FileSystemNameBuffer,nFileSystemNameSize)) {
			const ptrdiff_t dif=uit-buf;
			if(dif>0L)
				buflen=dif-1;
			}
		return buflen;
		}
	else
		buf[0]='\0';	
	return 0;		
	}
inline int getdrivestr(UINT drivetype,char *buf) {
	const char *const drivestr[]={"UNKNOWN", "NO_ROOT_DIR", "REMOVABLE", "FIXED", "REMOTE", "CD/DVD", "RAMDISK"};
	return sprintf(buf,"%s ",drivestr[drivetype]);
	}
inline  OFFTYPE getvolumesize(const char *volume) {
	HANDLE filehandle=MY_INVALID_HANDLE_VALUE;
	if((filehandle=opennothing(volume))!=MY_INVALID_HANDLE_VALUE ) {
		OFFTYPE len=getdisklength(filehandle);
		CloseHandle(filehandle);
		return len;
		}
	messagebuf[0]=0;
	return (OFFTYPE) -1;
	}
int isfloppy(const char *volume,char *buf) {
int buflen=0;
HANDLE han;
         if ((han = CreateFile(volume, 0, FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0)) != INVALID_HANDLE_VALUE)
         {
            DISK_GEOMETRY Geom[20];
            DWORD cb;

/* taken from: http://support.microsoft.com/kb/163920 */
            if (DeviceIoControl (han, IOCTL_DISK_GET_MEDIA_TYPES, 0, 0,
                                 Geom, sizeof(Geom), &cb, 0)
                && cb > 0)
            {
               switch (Geom[0].MediaType)
               {
               case F5_1Pt2_512: // 5.25 1.2MB floppy
               case F5_360_512:  // 5.25 360K  floppy
               case F5_320_512:  // 5.25 320K  floppy
               case F5_320_1024: // 5.25 320K  floppy
               case F5_180_512:  // 5.25 180K  floppy
               case F5_160_512:  // 5.25 160K  floppy
               case F3_1Pt44_512: // 3.5 1.44MB floppy
               case F3_2Pt88_512: // 3.5 2.88MB floppy
               case F3_20Pt8_512: // 3.5 20.8MB floppy
               case F3_720_512:   // 3.5 720K   floppy
		 {
		const char flopstr[]="FLOPPY ";
		const int floplen=sizeof(flopstr)-1;
		memcpy(buf,flopstr,floplen);
		buf[floplen]='\0';
		buflen=floplen;
		break;};
		default: 		break;
		} };
	 CloseHandle(han);
	}
//messagebuf[0]=0;
return buflen;
}
int diskinfo(const char *lastdrive,const char *volume,char *buf,const int maxbuf,bool noise) {
	int buflen=0;
 		UINT  drivetype=GetDriveType( lastdrive);
		buflen+=getdrivestr(drivetype,buf+buflen);
		if(noise||drivetype!=5) { 
			OFFTYPE size=getvolumesize(volume);
			if(size>(OFFTYPE)-1L)
				buflen+=sprintf(buf+buflen, OFFPRINT " ",size);
			if(int vollen=volumenamefs(lastdrive,buf+buflen,maxbuf-buflen)) {
				buflen+=vollen;
				}
			}
	return buflen;
	}
int  getvolumegegs(char *volume,int namelen,char *buf,int maxbuf) {
	int lastdrivepos,buflen=0;

   if((buflen=getvolpaths(volume,namelen,buf,&lastdrivepos))) {
   	int enddrive=buflen++;
	const char *lastdrive= buf+lastdrivepos;
	int added=isfloppy(volume,buf+buflen);
	if(added) 
		buflen+=added;
	else {

buflen+=diskinfo(lastdrive,volume,buf+buflen,maxbuf-buflen,false);
/*
 		UINT  drivetype=GetDriveType( lastdrive);
		buflen+=getdrivestr(drivetype,buf+buflen);
		if(drivetype!=5) { 
			OFFTYPE size=getvolumesize(volume);
			if(size>(OFFTYPE)-1L)
				buflen+=sprintf(buf+buflen, OFFPRINT " ",size);
			if(int vollen=volumenamefs(lastdrive,buf+buflen,maxbuf-buflen)) {
				buflen+=vollen;
				}
			}
*/
		}
	buf[enddrive]=' ';	
	}
else {
		memcpy(buf,volume,namelen);
		buf[buflen=namelen]=' ';
		int added=isfloppy(volume,buf+ ++buflen);
		if(added) 
			buflen+=added;
		else {
			OFFTYPE size=getvolumesize(volume);
			if(size>(OFFTYPE)-1L)
				buflen+=sprintf(buf+buflen, OFFPRINT " ",size);
			}
	}

return buflen;
};
inline BOOL hasfinds(void) {
	return success;
	};
};

Volumes *getvolptr() {
	static Volumes *volptr=new Volumes();
	if(volptr&&!volptr->hasfinds()) {
		delete volptr; 
		volptr=NULL;
		}
	return volptr;
	}

/*
	for(int partiter=0;diskiter<MAXDRIVES;drivenr++,pariter++) {
			const char hard[]="\\\\.\\Harddisk",part[]="Partition";
			char *tmp=devices[drivenr]=assigndev(sizeof(hard)+sizeof(part)+8);	
			memcpy(tmp,hard,sizeof(hard)-1);

			}
*/
int getpartitions(char **devices) {
int diskiter=0;
	messagebuf[0]=0;
#ifdef DUBBLEFLOP /*Floppies already returned by FindFirst/NextVolumeA*/
	if(GetDriveType("A:\\")>1) 
	{
			devices[diskiter]=assigndev(7*sizeof(char));
			memcpy(devices[diskiter++],"\\\\.\\A:",7*sizeof(char));
	}	
	if(GetDriveType("B:\\")>1) 
	{
			devices[diskiter]=assigndev(7*sizeof(char));
			memcpy(devices[diskiter++],"\\\\.\\B:",7*sizeof(char));
	}	
#endif
{
if(Volumes *volptr=getvolptr()) {
/* The following code concerning volume names is a modification of the example given at:	*
 *	http://msdn.microsoft.com/en-us/library/cc542456(VS.85).aspx 				*/
    VolNameType  VolumeName[MAX_PATH];
    DWORD  Error                = ERROR_SUCCESS;
    HANDLE FindHandle           = INVALID_HANDLE_VALUE;
    FindHandle = volptr->FindFirstVolumeA(VolumeName, sizeof(VolumeName));

if (FindHandle == INVALID_HANDLE_VALUE)
    {
        Error = GetLastError();
        erroruit( "FindFirstVolumeA failed with error code 0x%X\n", (unsigned int)Error);
    }
else {
    for (;;)
    {
    BOOL   Success              = FALSE;
    	size_t  Volnamelen = strlen(VolumeName) - 1;
        if (VolumeName[0]     !=   '\\' ||
            VolumeName[1]     !=   '\\' ||
            VolumeName[2]     !=   '?'  ||
            VolumeName[3]     !=   '\\' ||
            VolumeName[Volnamelen] !=   '\\') 
        {
            Error = ERROR_BAD_PATHNAME;
            erroruit( "FindFirstVolume/FindNextVolume returned a bad path: %s\n", VolumeName);
            break;
        }

        VolumeName[Volnamelen++] =   '\0';
	DEBUGGING("Volume: %s", VolumeName);
	int labellen=MAX_PATH;

	char **tmpdev=(char **)malloc(labellen*sizeof(char)+sizeof(char *));
	*tmpdev= (char *)malloc((Volnamelen)*sizeof(VolNameType));
	devices[diskiter]= (char *)&tmpdev[1];
	labellen=  volptr->getvolumegegs(VolumeName,Volnamelen-1, devices[diskiter],labellen);
	DEBUGGING(" getvolumegegs %s\n",devices[diskiter])

	diskiter++;

	memcpy(*tmpdev,VolumeName,sizeof(VolNameType)*Volnamelen);
        Success = volptr->FindNextVolumeA(FindHandle, VolumeName, sizeof(VolumeName));
/*Windows 2000 Professional*/
        if ( !Success ) 
        {
            Error = GetLastError();

            if (Error != ERROR_NO_MORE_FILES) 
            {
                erroruit( "FindNextVolumeA failed with error code 0x%X\n", (unsigned int)Error);
                break;
            }
            Error = ERROR_SUCCESS;
            break;
        }
    }
    volptr->FindVolumeClose(FindHandle);
    FindHandle = INVALID_HANDLE_VALUE;

	DWORD driv=	GetLogicalDrives();
char mpoint[]="a:\\";
	for(int i=0;i<32;i++) 
	 	if(driv&(1<<i)) {
		const int volsize= MAX_PATH;
		char volume[volsize];
		mpoint[0]='a'+i;
	      if(! volptr->GetVolumeNameForVolumeMountPointA( mpoint, volume,   volsize               )) {
	const int maxshow=260;
	char **tmpdev=(char **)malloc(maxshow*sizeof(char)+sizeof(char *));
	*tmpdev= (char *)malloc(7);
	char *buf=devices[diskiter++]= (char *)&tmpdev[1];
	sprintf(*tmpdev,"\\\\.\\%c:",'A'+i);

				memcpy(buf,mpoint,3);
				buf[3]=' ';
int buflen=4;
buflen+=volptr->diskinfo(mpoint,*tmpdev,buf+buflen,maxshow-buflen,true);
/*
BOOL WINAPI GetVolumeInformation(
  __in_opt   LPCTSTR lpRootPathName,
  __out      LPTSTR lpVolumeNameBuffer,
  __in       DWORD nVolumeNameSize,
  __out_opt  LPDWORD lpVolumeSerialNumber,
  __out_opt  LPDWORD lpMaximumComponentLength,
  __out_opt  LPDWORD lpFileSystemFlags,
  __out      LPTSTR lpFileSystemNameBuffer,
  __in       DWORD nFileSystemNameSize
);
				UINT  drivetype=GetDriveType( mpoint);
				int buflen=volptr->getdrivestr(drivetype,buf+4)+4;
				buf[buflen++]=' ';
				OFFTYPE size=volptr->getvolumesize(*tmpdev);
				sprintf(buf+buflen,OFFPRINT,size);
*/
				}
		}
	}
}
else {
	DWORD driv=	GetLogicalDrives();
	for(int i=0;i<32;i++) 
		if(driv&(1<<i)) {
			devices[diskiter]=assigndev(7);	
			sprintf(devices[diskiter++],"\\\\.\\%c:",'A'+i);
			}
}
 }
return diskiter;
}

int getNTdrives(char ***devicesptr) {
	HANDLE filehandle;
	const int maxhard=MAXDRIVES/2;
char **devices=(char **)malloc(sizeof(char *)* MAXDRIVES );
if(!devices)
	return -1;

*devicesptr=devices;
int diskiter=0;
	for(int physnr=0;physnr<maxhard;physnr++) {
		const int end=17;
		const char disk[]="\\\\.\\PHYSICALDRIVE";
		char *dev=devices[diskiter]=assigndev(end+4+1+22);	
		char *tmp=(char *)malloc(end+5);
		strcpy(dev,disk);
		int pos=sprintf(dev+end,"%d",physnr)+end;	
		strcpy(tmp,dev);
		((char **)dev)[-1]=tmp;
		if((filehandle=opennothing(dev))!=MY_INVALID_HANDLE_VALUE) {
			long long len;
			if((len=getdisklength(filehandle) )>=0) {
				sprintf(dev+pos," " OFFPRINT " ",len);
				}
			CloseHandle(filehandle);
			diskiter++;
		}	


	}

int partnr=getpartitions(devices+diskiter); 
diskiter+=partnr;
if(diskiter<1)
	free(devices);
return diskiter;
}

#endif /* USE_WINFILE */

#ifdef __linux__

static const char procpart[]="/proc/partitions";

bool isblockdev(const char * const devname,int maj,int min) {
		struct stat st;	
		if(stat(devname,&st)==0) {
			if(S_ISBLK(st.st_mode)) {
				int fmaj=major(st.st_rdev), fmin=minor(st.st_rdev);
				if(fmaj==maj&&fmin==min) {
					return true;
					}
				else 
					fprintf(stderr,"%s different Major or Minor numbers then given in %s\n",devname,procpart);
				}
			else  
				fprintf(stderr,"%s no block device\n",devname);
			}
		return false;
		
	};
int getLinuxdrives(char ***devicesptr) {
	FILE * const han=fopen(procpart,"rb");
	char buf[1024];
	long long bl;
	long maj,min;
	char name[512];
	long line=0,maxline;
	char **devices;
	if(!han) {
			fprintf(stderr,"Can't open %s: %s\n",procpart,strerror(errno));
			return -1;
			}
	
	for(maxline=0;fgets(	buf,1024,han);maxline++) 
			;
	fseek(han,0L,SEEK_SET);
	if(!fgets(	buf,1024,han)) {
			fprintf(stderr,"%ld: read from %s failed\n",line,procpart);
			fclose(han);
			return -1;
			}
	line++;
	if(!fgets(	buf,1024,han)) {
			fprintf(stderr,"%ld: read from %s failed\n",line,procpart);
			fclose(han);
			return -1;
			}
	devices=(char **)malloc((maxline-2)*sizeof(char *));
	if(!devices) {
			fprintf(stderr,__FILE__ "%d malloc failed\n",__LINE__);
			fclose(han);
			return -1;
			}
	*devicesptr=devices;
	while(fgets(	buf,1024,han)) 
	{
		 line++;
		if(sscanf(buf,"%ld%ld%lld%s",&maj,&min,&bl,name)!=4)
			fprintf(stderr,"%s line %ld: sscanf failed\n",procpart,line);

		const int len=strlen(name);
		const char devblock[]= "/dev/block/";
		const int devblocklen=sizeof(devblock);
		char * const devname= assigndev(len+devblocklen);
		strcpy(devname,devblock);
		strcpy(devname+devblocklen-1,name);
		if(!isblockdev(devname,maj,min)) {
			strcpy(devname+5,name);
			if(!isblockdev(devname,maj,min)) {
				fprintf(stderr,"%s major=%ld minor=%ld does not exist",devname,maj,min);
				}
			}
                const int num=line-2;
                devices[num]=devname;
	}


	fclose(han);
	if(line<2) 
		free(devices);
	return (line-1);
	}
#endif /* __linux__ */


int Editor::selectdisk(void) {
	int showwidth=70;
	static int disknr=0; /*total number of 'drives' (physical+logical) */
	static char **devices=NULL;
	INCDEST();
if(disknr) /*Interrupted by signal?*/
	freearray(devices,disknr); 	
#ifdef USE_WINFILE
#ifdef BIOSDISK
if(doswin()) {
DEBUGGING("doswin ")
	if((disknr=getbiosdrives(&devices))<1) {
		DECDEST();
			return disknr;
			}
	}
else 
#endif
{

	if((disknr=getNTdrives(&devices))<1) {
		DECDEST();
		return disknr;
		}

}
#else // USE_WINFILE
#ifdef __linux__
	if((disknr=getLinuxdrives(&devices))<1) {
		DECDEST();
		return disknr;
		}

#endif // __linux__
#endif // USE_WINFILE
DECDEST();
	static int take=0;
	int ant;
	if(take<0||take>=disknr)
		take=0;
	char label[]="Select drive and Press Enter";
DEBUGGING("Before selectitemindex ")
	while((ant=selectitemindex(label,devices,disknr,take,showwidth))!=INT_MAX) {
//	while((ant=selectitemindex(label,devices,disknr,take))!=INT_MAX) {
			if(ant==(INT_MAX-1)) {
				menuresized();
				wrefresh(editscreen);
				}
			else {
					editup();
					if(ant>=0&&ant<disknr) {
						take=ant;
						char *disk=devices[ant];
						char **devptr=(char **)disk;
						if(*--devptr) 
							disk=*devptr;
						if(!editfile(disk)) {
								beep();	
#ifndef _WIN32
						if(errno)
							message("Can't open %s: %s",disk,strerror(errno));
						else
#endif
								showmessage();
						wrefresh(editscreen);
								freearray(devices,disknr);
								return -1;
								}
					freearray(devices,disknr);

					return ant;
						}
					else {	
						erroruit("selectdisk: %d out of range",ant);
						freearray(devices,disknr);
						return -2;
						}
				 }
		}
	editup();
	freearray(devices,disknr);
	return 0;
	}

#endif
#ifndef __MINGW32__
              #include <grp.h>
       #include <pwd.h>
       #include <sys/types.h>
#endif


#define perms(x,el) ((x&(1<<el))?(permsym[el]):'-')
#include "dialog.h"


#ifdef _WIN32


extern char * expanddisk(const char * const name,char *const filename) ;
inline bool  driveletter( const char * const name) {return (name[3]=='\\'&&name[5]==':'&&!name[6]&&(name[2]=='.'||name[2]=='?')) ; }

inline BOOL diskvolume(char *Name,int len) {
// return (len>10&&(Name[0] == '\\' && Name[1] ==  '\\' && (Name[2] ==  '?' ||Name[2]=='.')&& Name[3]  ==  '\\' )&&(!memcmp(Name+4,"Volume",6)));
 return (len>3&&(Name[0] == '\\' && Name[1] ==  '\\' && (Name[2] ==  '?' ||Name[2]=='.')&& Name[3]  ==  '\\' ));
}

char * expanddisk(const char * const name,char *const filename) {
 if(driveletter(name)
#ifdef RUNONWIN95
&&!doswin()
#endif
) {
     if(Volumes *volptr=getvolptr()) {
	const char mpoint[]={name[4],':','\\',0};
        if( volptr->GetVolumeNameForVolumeMountPointA( mpoint, filename,   MAX_PATH )) {
		const int end=strlen(filename);
		filename[end-1]='\0';
		return filename;
		}
	}
     }
     strcpy(filename,name);
	return filename;
	}

#endif

constexpr const char *filedataformat(const unsigned int) {return " %u %s %s " OFFPRINT;};
constexpr const char *filedataformat(const unsigned long) {return " %ul %s %s " OFFPRINT;};
constexpr const char *filedataformat(const unsigned long long) {return " %ull %s %s " OFFPRINT;};
int Editor::filedata(void) {
	memorize();
	if( !mem.isnewfile()) {
	int maxbuf= BEDSMAXPATH+1;
	char buf[maxbuf];
while(1) {
		int buflen;
		int width,res=0;
#ifndef _WIN32
		const char *permsym="xwrxwrxwr";
char *ptr;
		struct tm *tp;
		int i,j=0;
		const char *user,*group;
		struct passwd *pas;
		struct group *groupst;
		char timestr[]=" %b %d %Y %H:%M";

		if(S_ISDIR(mem.statdata.st_mode))
			buf[j++]='d';
		else
			buf[j++]='-';
		for(i=8;i>=0;i--)
			buf[j++]=perms(mem.statdata.st_mode,i);
		if((pas=getpwuid(mem.statdata.st_uid)))
			user=pas->pw_name;
		else
			user="????";
		if((groupst=getgrgid(mem.statdata.st_gid)))
			group=groupst->gr_name;
		else
			group="????";
		tp=localtime(&mem.statdata.st_mtime);

		ptr=buf+j;
//		j=sprintf(ptr," " SIZE_TPRINT " %s %s " OFFPRINT,mem.statdata.st_nlink,user,group,mem.diskfilesize);
		j=sprintf(ptr,filedataformat(mem.statdata.st_nlink),mem.statdata.st_nlink,user,group,mem.diskfilesize);
	if(mem.diskfilesize!=mem.filesize) {
		j+=sprintf(ptr+j," (" OFFPRINT ")",mem.filesize);
		}
	else {
		if(modified)
			ptr[j++]='*';
		}
		ptr+=j;
		strftime(ptr,80,timestr,tp);
		buflen=ptr-buf+strlen(ptr);
#else
#ifdef BIOSDISK
if(mem.infodisk) {
		OFFTYPE siz= (OFFTYPE)mem.infodisk->cylinders*mem.infodisk->heads* mem.infodisk->sectors* mem.infodisk->BytesPerSector;
		if(siz==mem.filesize)
		buflen=sprintf(buf,"%ld*%ld*%ld*%hd=%lld",mem.infodisk->cylinders,mem.infodisk->heads, mem.infodisk->sectors, mem.infodisk->BytesPerSector,siz);
	else
		buflen=sprintf(buf,"%ld*%ld*%ld*%hd="OFFPRINT " (size used: " OFFPRINT ")",mem.infodisk->cylinders,mem.infodisk->heads, mem.infodisk->sectors, mem.infodisk->BytesPerSector,siz,mem.filesize);
		}
else
#endif
{

//typedef WINBASEAPI BOOL WINAPI (*GetVolumePathNamesForVolumeNameAType)(LPCSTR,LPSTR,DWORD,PDWORD);


char *ptr;
if(diskvolume(mem.filename,mem.filenamelen)) {
if(Volumes *volptr=getvolptr()) {
char *lastdrive;
int enddrive;
if(mem.filenamelen>10&&!memcmp(mem.filename+4,"Volume",6)) {
	#ifdef OOKPUNT
		char waspunt= mem.filename[2];
		mem.filename[2]='?';
	#endif
	int lastdrivepos;
	 buflen=volptr->getvolpaths(mem.filename,mem.filenamelen,buf,&lastdrivepos);
	#ifdef OOKPUNT
		mem.filename[2]=waspunt;
	#endif
	if(!buflen|| !buf[0])
		goto otherdisk;
	lastdrive= buf+lastdrivepos;
   	enddrive=buflen++;
	}
else { 
if(driveletter(mem.filename)) {
	sprintf(	buf,"%c:\\",mem.filename[4]);
	enddrive=3;
	buflen=enddrive+1;
	lastdrive=buf;
	}
else
		goto otherfile;
	}

 	UINT  drivetype=GetDriveType( lastdrive);

	if(int len=volptr->isfloppy(mem.filename,buf+buflen) )
		buflen+=len;
	else
		if(drivetype>=0&&drivetype<7)
			buflen+=volptr->getdrivestr(drivetype,buf+buflen);
	const DWORD maxvolume=MAX_PATH*3/2;
	char Volumename[maxvolume];
	int volnamelen=volptr->volumenamefs(lastdrive,Volumename,maxvolume); 
		buf[enddrive]=' ';	
		buflen+=sprintf(buf+buflen,OFFPRINT,mem.diskfilesize);
	if(mem.diskfilesize!=mem.filesize) {
		buflen+=sprintf(buf+buflen," (" OFFPRINT ")*",mem.filesize);
		}
	else {
		if(modified) {
			buf[buflen++]='*';
			buf[buflen]='\0';
			}

		}
		width=minnum(	thecols,maxnum(volnamelen,maxnum(mem.filenamelen,buflen))+4);
begindialog(9,width,4);
		mvwaddstr(takescr,1,2,Volumename);
		mvwaddstr(takescr,3,2,buf);
		mvwaddstr(takescr,5,2,mem.filename);
		oke(7,width/2-2);
		enddialog(res,res);
		if(res==INT_MAX)  {
res=0;
			menuresized();
			wrefresh(editscreen);
			continue;
			}
		break;	
		} 
else {	
otherdisk:
	buflen=	23;
	strcpy(buf,"*** No mountpoints *** ");
	}
ptr=buf+buflen;
#ifdef _WIN32
ptr+=sprintf(ptr, OFFPRINT,mem.diskfilesize);
#endif
}
	else {	
#ifdef _WIN32
otherfile:
extern int filegeg(char *name,char *buf);
int end=filegeg(mem.filename,buf);
ptr=buf+end;
#else
	struct tm *tp;
	int j=0;
	if(mem.statdata.st_ino) {
		char timestr[]=" %b %d %Y %H:%M";	
		if(S_ISDIR(mem.statdata.st_mode))
			buf[j++]='d';
		else
			buf[j++]='-';
		const char permsym[]="xwrxwrxwr";
		int endperm=6;
		for(int i=8;i>=endperm;i--)
			buf[j++]=perms(mem.statdata.st_mode,i);
		buf[j++]=' ';
		tp=localtime(&mem.statdata.st_mtime);

		ptr=buf+j;
		strftime(ptr,80,timestr,tp);
		ptr= strlen(ptr)+ptr;
		*ptr++=' ';
		}
	else
		ptr=buf;

#endif
	}
#ifdef _WIN32
if(ptr==buf)
#endif
ptr+=sprintf(ptr, OFFPRINT,mem.diskfilesize);
if(mem.diskfilesize!=mem.filesize) {
	ptr+=sprintf(ptr," (" OFFPRINT ")*",mem.filesize);
	}
else {
	if(modified) {
		*ptr++='*';
		*ptr++='\0';
		}
	}
buflen=ptr-buf;
}
#endif

		width=minnum(	thecols,maxnum(mem.filenamelen,buflen)+4);
begindialog(8,width,4);
		mvwaddstr(takescr,1,2,buf);
		mvwaddstr(takescr,3,2,mem.filename);
		  oke(6,width/2-2);
		  enddialog(res,res);
		if(res==INT_MAX)  {
res=0;
			menuresized();
			wrefresh(editscreen);
			continue;
			}
		break;
		};
		editup();
		}
	else {
		message("%s " OFFPRINT "%c",mem.filename,mem.filesize, modified?'*':' ');
		return 0;
		}
	return 0;
}
#undef perms




