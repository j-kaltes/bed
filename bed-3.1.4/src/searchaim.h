#ifndef SEARCHAIM_H
#define SEARCHAIM_H 1
#include "offtypes.h"
#include "screenpart.h"
typedef struct {
	uint8_t  sense;
	int doreplace;
	int len;
	int align;

	signed char *str;
	int matchlen;
//	OFFTYPE start,end;
	OFFTYPE start;
	ScreenPart *datashown;
	} SearchAim ;


#endif //SEARCHAIM_H

