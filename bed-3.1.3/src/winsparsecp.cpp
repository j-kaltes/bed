
#if defined(USE_WINFILE) 
//#define _WIN32_WINNT		0x0500		// Windows 2000 or later

#include <windows.h>
#include <stdio.h>
template <typename T1,typename T2> min(T1 one,T2 two) {
	return one<two?one:two;
	}

#include "winsparsecp.h"

void PrintError(DWORD dwErr) {
  char szMsg[256];
  DWORD dwFlags = FORMAT_MESSAGE_IGNORE_INSERTS |
                  FORMAT_MESSAGE_MAX_WIDTH_MASK |
                  FORMAT_MESSAGE_FROM_SYSTEM;

  if (!::FormatMessage(dwFlags, NULL, dwErr, 0, szMsg, sizeof(szMsg), NULL)) strcpy(szMsg, "Unknown error.");
  printf(szMsg);
  printf("\n");
}

bool CopyRange(HANDLE hFrom, LARGE_INTEGER iPos, LARGE_INTEGER iSize, HANDLE hTo, LARGE_INTEGER tooff) {
  BYTE buf[64*1024];

if(!SetFilePointerEx( hFrom, iPos, NULL,FILE_BEGIN)) {
	PrintError(GetLastError());
	return false;
	}
if(!SetFilePointerEx( hTo, tooff, NULL,FILE_BEGIN)) {
	PrintError(GetLastError());
	return false;
	}

DWORD dwBytesRead;
for(auto len=iSize.QuadPart;len > 0;len -= dwBytesRead) {
	if(!::ReadFile(hFrom, buf, (DWORD)min(sizeof(buf), len), &dwBytesRead, NULL)) {
		PrintError(GetLastError());
		return false;
		}
	DWORD dwBytesWritten;
	if(!::WriteFile(hTo, buf, dwBytesRead, &dwBytesWritten, NULL)) {
		PrintError(GetLastError());
		return false;
		}
	if (dwBytesWritten < dwBytesRead)
		return false;
	}
return true;
}




int sparsesize(HANDLE hOutFile, const LONGLONG  size) {
	LARGE_INTEGER iFileSize{.QuadPart=size};
	if(!SetFilePointerEx( hOutFile, iFileSize, NULL,FILE_BEGIN)) {
		PrintError(GetLastError()); 
		return -2;
		}
	if (!SetEndOfFile(hOutFile))  {
		PrintError(GetLastError());
		return -3;
		}
	return 0;
	}

bool mksparse(HANDLE hOutFile) {
	DWORD dwTemp;
	if (!DeviceIoControl(hOutFile, FSCTL_SET_SPARSE, NULL, 0, NULL, 0, &dwTemp, NULL)) {
		PrintError(GetLastError());
		return false;
		}
	return true;
	}
/*
typedef struct _FILE_ALLOCATED_RANGE_BUFFER {
  LARGE_INTEGER FileOffset;
  LARGE_INTEGER Length;
} FILE_ALLOCATED_RANGE_BUFFER, *PFILE_ALLOCATED_RANGE_BUFFER;
typedef union _LARGE_INTEGER {
  struct {
    DWORD LowPart;
    LONG  HighPart;
  } DUMMYSTRUCTNAME;
  struct {
    DWORD LowPart;
    LONG  HighPart;
  } u;
  LONGLONG QuadPart;
} LARGE_INTEGER;
*/
int sparsecp(HANDLE hInFile,LONGLONG instart, LONGLONG inlen,HANDLE hOutFile) {
//	if(!mksparse(hOutFile)) return -1;
	FILE_ALLOCATED_RANGE_BUFFER queryrange {.FileOffset{.QuadPart{instart}},.Length{.QuadPart{inlen}}};
	LONGLONG inend=instart+inlen;
	BOOL br;
	do {
		FILE_ALLOCATED_RANGE_BUFFER ranges[4096];
		DWORD nbytes;
		br = ::DeviceIoControl(hInFile, FSCTL_QUERY_ALLOCATED_RANGES, &queryrange, sizeof(queryrange), ranges, sizeof(ranges), &nbytes, NULL);
		if (!br) {
			auto dr = ::GetLastError();
			if (dr != ERROR_MORE_DATA)  {
				PrintError(dr);
				return -1;
				}
		}

		DWORD n = nbytes / sizeof(FILE_ALLOCATED_RANGE_BUFFER);		// Calculate the number of records returned
		for (int i=0; i<n; i++){		// Main loop
			if(!CopyRange(hInFile, ranges[i].FileOffset, ranges[i].Length, hOutFile, LARGE_INTEGER {.QuadPart{ranges[i].FileOffset.QuadPart-instart}}))
				return -3;
			}

		// Set starting address and size for the next query
		if (!br && n > 0) {
			queryrange.FileOffset.QuadPart = ranges[n-1].FileOffset.QuadPart + ranges[n-1].Length.QuadPart;
			queryrange.Length.QuadPart = inend - queryrange.FileOffset.QuadPart;
		}
	} while (!br);	// Continue loop if ERROR_MORE_DATA



//return sparsesize(hOutFile,inlen);
return 0;
}

//bool sparsecp(HANDLE hInFile,LONGLONG instart, LONGLONG inlen,HANDLE hOutFile) {

template <int nr>
DWORD copyel(HANDLE hInFile,HANDLE hOutFile,const int len) {
	BYTE buf[nr];
	DWORD dwBytesRead, dwBytesWritten;
	if (!ReadFile(hInFile, buf, len, &dwBytesRead, NULL))  {
		PrintError(GetLastError());
		return -1;
		}
	if(dwBytesRead) {
		if (!WriteFile(hOutFile, buf, dwBytesRead, &dwBytesWritten, NULL)) {
			PrintError(GetLastError());
			return -1;
			}

		if (dwBytesWritten < dwBytesRead)  {
			return -1;
			}
	}
	return  dwBytesRead;
	}
int nonsparsecp(HANDLE hInFile,LONGLONG inoff, LONGLONG inlen,HANDLE hOutFile) {
	constexpr int buflen=64*1024; 


	LARGE_INTEGER offset{.QuadPart=inoff};
	if(!SetFilePointerEx( hInFile, offset, NULL,FILE_BEGIN)) {
		PrintError(GetLastError());
		return -2;
		}
	int num=inlen/buflen;
	for(int i=0;i<num;i++) {
		if(copyel<buflen>(hInFile,hOutFile,buflen)!=buflen)
			return -3;
		}
	const int over=inlen%buflen;
	if(copyel<buflen>(hInFile,hOutFile,over)!=over)
		return -4;
	return 0;
	}
/*	
bool cansparse(const char *name) {
		char szPath[MAX_PATH], szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFName[_MAX_FNAME], szExt[_MAX_EXT];
		LPSTR pszName;
		char szVolName[MAX_PATH], szFSName[MAX_PATH];
		DWORD dwSN, dwMaxLen, dwVolFlags;
		if (!::GetFullPathName(name, MAX_PATH, szPath, &pszName)) 
			return false;
		_splitpath(szPath, szDrive, szDir, szFName, szExt);
		if(szDrive[0]) {
			strcat(szDrive, "\\");
			if (!::GetVolumeInformation(szDrive, szVolName, MAX_PATH, &dwSN, &dwMaxLen, &dwVolFlags, szFSName, MAX_PATH)) 
				return false;
			return  dwVolFlags & FILE_SUPPORTS_SPARSE_FILES;
			}
		return false;
		}
bool copy(const char *infile,const char *outfile) {

    bool dosparse=cansparse(infile)&&cansparse(outfile);

    if(HANDLE hInFile = CreateFile(infile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL); hInFile==INVALID_HANDLE_VALUE) {
	 PrintError(GetLastError());
	return false;
	}
else {

   bool noproblem=true;
	if(BY_HANDLE_FILE_INFORMATION bhfi; !GetFileInformationByHandle(hInFile, &bhfi)) {
		 PrintError(GetLastError());
		return false;
		}
	else {
		dosparse = dosparse&&(bhfi.dwFileAttributes & FILE_ATTRIBUTE_SPARSE_FILE);	
		}

    if(HANDLE hOutFile = CreateFile(outfile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);hOutFile == INVALID_HANDLE_VALUE)  {
		 PrintError(GetLastError());
		return false;
		}
	else {
		if(!dosparse||whole<sparsecp>(hInFile,hOutFile)==-1) 
			noproblem=whole<nonsparsecp>(hInFile,hOutFile)==0;

    		CloseHandle(hOutFile);
		}
    CloseHandle(hInFile);
   return noproblem;
  }
}

int main(int argc,char **argv) {
	copy(argv[1],argv[2]);
	}
*/
#endif
