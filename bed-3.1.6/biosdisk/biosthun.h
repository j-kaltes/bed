#ifndef _THUNK_H_
#define _THUNK_H_
#include <windows.h>
#define DISKREAD 1
#define DISKWRITE 2
#define SIGN 0
#define IDENT 19283L
#define GETMEM 3
#define DISKREADFAILED -1L
typedef char DISKIDENT;
typedef DWORD SIZE_T;
typedef struct {DWORD low;DWORD high;} OFFSETTYPE;
const OFFSETTYPE OFFSETINVALID = {0xFFFFFFFFL,0xFFFFFFFFL};
/*
struct disktrans {
	SIZETYPE size;
	OFFSETTYPE offset;
	void *buf;
	BYTE drive;
	};
*/
#define tdisk(gegs) gegs[0]
#define tbuffer(gegs) gegs[1]
#define tsize(gegs) gegs[2]
#define tofflow(gegs) gegs[3]
#define toffhi(gegs) gegs[4]
#define BUFSIZE (5*sizeof(DWORD))
#endif
