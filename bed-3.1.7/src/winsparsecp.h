#ifndef WINSPARSECP_H
#include <windows.h>
#define WINSPARSECP_H
extern int sparsesize(HANDLE hOutFile, const LONGLONG  size) ;
extern bool mksparse(HANDLE hOutFile) ;
extern int sparsecp(HANDLE hInFile,LONGLONG instart, LONGLONG inlen,HANDLE hOutFile); 
extern int nonsparsecp(HANDLE hInFile,LONGLONG inoff, LONGLONG inlen,HANDLE hOutFile); 
extern void PrintError(DWORD dwErr) ;
template <int (*F)(HANDLE,LONGLONG,LONGLONG,HANDLE)>
int whole(HANDLE hInFile,HANDLE hOutFile) {
	LARGE_INTEGER iFileSize;
	if (!GetFileSizeEx(hInFile,&iFileSize)) {
		PrintError(GetLastError());
		return false;
		}
	return  F(hInFile,0L, iFileSize.QuadPart,hOutFile); 
}

#endif
