#include "defines.h"
/*     Bed a menu-driven multi dataformat binary editor for Linux            */
/*     Copyright (C) 1998 Jaap Korthals Altes <binaryeditor@gmx.com>       */
/*                                                                           */
/*     This program is free software; you can redistribute it and/or modify  */
/*     it under the terms of the GNU General Public License as published by  */
/*     the Free Software Foundation; either version 2 of the License, or     */
/*     (at your option) any later version.                                   */
/*                                                                           */
/*     This program is distributed in the hope that it will be useful,       */
/*     but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*     GNU General Public License for more details.                          */
/*                                                                           */
/*     You should have received a copy of the GNU General Public License     */
/*     along with this program; if not, write to the Free Software           */
/*     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.             */
/* Fri Dec  8 22:14:27 2000                                                  */
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "type.h"
#include "screenpart.h"
#include "repeat.h"
#include "contain.h"
#include "system.h"

//#define errormessage(form...) snprintf(mes,80,##form)

//static int newlinenr;
#include "varmacro.h"
#ifdef OLDGCCVARMACRO
#define errormessage(form...) snprintf(mes,80,##form) VARMACRO
#else
#define errormessage(...) snprintf(mes,80,__VA_ARGS__) 
#endif
/*
#ifdef OLDGCCVARMACRO
#define errormessage(form...) snprintf(mes+sprintf(mes,"%d:",newlinenr),80,##form) VARMACRO
#else
#define errormessage(...) snprintf(mes+sprintf(mes,"%d:",newlinenr),80,__VA_ARGS__) 
#endif
*/
#define anincrement 2
#define spacechar ' '
int datatype2str(char *gegs,int maxgegs,ScreenPart *sub,int depth) ;
/* New Format:
Composed name,datb,fb,type, nr,children,convstr,filters,label,apart,space)



*/
int common2str(char *gegs,int maxgegs,ScreenPart *sub) {
		int gegslen=0;
		gegslen+=sprintf(gegs+gegslen,"%s(%d,%d,%d",sub->getname(),sub->datbytes(),sub->bytes,sub->type);

		return gegslen;
		}
int commonend(char *gegs,int maxgegs,ScreenPart *sub) {
		int gegslen=0;
		gegslen+=sprintf(gegs+gegslen,",\"%s\",[", sub->getconvstr());
		gegslen+=sub->filtersstring(gegs+gegslen);
		gegslen+=sprintf(gegs+gegslen,"]");
		if(sub->userlabel)
			gegslen+=sprintf(gegs+gegslen,",\"%s\"",sub->userlabel);
		else
			gegslen+=sprintf(gegs+gegslen,",\"\"");
		gegslen+=sprintf(gegs+gegslen,",%d,%d)",sub->apart,sub->spaceafter);
		return gegslen;
		}

int repeat2str(char *gegs,int maxgegs,Repeat *sub,int depth) {
	int gegslen=0;
	gegslen+=snprintf(gegs+gegslen,maxgegs-gegslen,",%d,[",sub->nrsubparts);
	gegslen+=datatype2str(gegs+gegslen,maxgegs-gegslen,sub->subpart(0),depth);
	gegslen+=snprintf(gegs+gegslen,maxgegs-gegslen,"]");
	return gegslen;
	}
/*
char newline[10];
int getnewline(void) {
	sprintf(newline,"\n");
	return (int)(((char *)memchr(newline,0,9))-newline);
	}
int newlinelen= getnewline();
*/
#ifdef _WIN32
char newline[]={0x0d,0x0a};
#else
char newline[]={0x0a};
#endif
int newlinelen= sizeof(newline);

#define ADDNEWLINE  gegslen+=addnltoar(gegs+gegslen);
/*
#ifdef _WIN32
#define ADDNEWLINE gegs[gegslen++]=0x0D; gegs[gegslen++]=0x0A;
#else
#define ADDNEWLINE gegs[gegslen++]='\n';
#endif
*/
int hassub2str(char *gegs,int maxgegs,HasSub *sub,int depth) {
	int gegslen=0;
	gegslen+=snprintf(gegs+gegslen,maxgegs-gegslen,",%d,[",sub->nrsubparts);
	if(sub->nrsubparts>0) {
ADDNEWLINE
		memset(gegs+gegslen,spacechar,depth);
		gegslen+=depth;
		gegslen+=datatype2str(gegs+gegslen,maxgegs-gegslen,sub->subpart(0),depth+anincrement);
		for(int i=1;i<sub->nrsubparts;i++) {
			gegs[gegslen++]=',';
			ADDNEWLINE
			memset(gegs+gegslen,spacechar,depth);
			gegslen+=depth;
			gegslen+=datatype2str(gegs+gegslen,maxgegs-gegslen,sub->subpart(i),depth+anincrement);
			}
		}
	gegslen+=snprintf(gegs+gegslen,maxgegs-gegslen,"]");
	return gegslen;
	}

int datatype2str(char *gegs,int maxgegs,ScreenPart *sub,int depth) {
	int gegslen=0;
	gegslen+=common2str(gegs+gegslen,maxgegs-gegslen,sub);	
	if(has_super(sub,Repeat)) 
		gegslen+=repeat2str(gegs+gegslen,maxgegs-gegslen,(Repeat *)sub,depth);
	else {	
		if(has_super(sub,HasSub))
			gegslen+=hassub2str(gegs+gegslen,maxgegs-gegslen,(HasSub *)sub,depth);
		}
	gegslen+=commonend(gegs+gegslen,maxgegs-gegslen,sub);	
	return gegslen;
	}

int datatype2str(char *gegs,int maxgegs,ScreenPart *sub) {
	int gegslen=0;
	ADDNEWLINE
	gegslen+=datatype2str(gegs+gegslen,maxgegs-gegslen,sub,anincrement); 
	gegs[gegslen++]=';';
	ADDNEWLINE
	gegs[gegslen]='\0';
	return gegslen;
	}
#include "editor.h"
int savedatatype(ScreenPart *part,FILEHANDLE handle) {
	const ssize_t maxgegs=(ssize_t)4096*1024L;
	char *gegs=(char *)mymalloc(maxgegs*sizeof(char));
	if(!gegs) {
		if(part->editor)
			part->editor->message("System error: can't malloc(" SIZE_TPRINT ")\n",maxgegs);
		return -1;
		}
	ssize_t gegslen=datatype2str(gegs,maxgegs,part); 
	writetofile(handle,gegs,gegslen);
	free(gegs);
	return 0;
	}
/*
int savedatatype(ScreenPart *part,FILEHANDLE handle) {
	static char *gegs=NULL;
	int gegslen=0;
	const int maxgegs=4096*1024;
	gegs=(char *)realloc(gegs,maxgegs*sizeof(char));
	if(!gegs) {
		if(part->editor)
			part->editor->message("System error: can't malloc(%d)\n",maxgegs);
		return -1;
		}
	gegslen+=datatype2str(gegs+gegslen,maxgegs-gegslen,part); 
	writetofile(handle,gegs,gegslen);
	free(gegs);gegs=NULL;
	return 0;
	}
*/
class Editor;

#define	checkend(ptr,len)\
	if(len<1||ptr[len]=='\n') {\
		message("unexpect end line in %s",ptr);\
		return NULL;\
		}
void skipsep(char *&ptr,size_t  &len) {
		len=strspn(ptr, "	 \012\015"); 
		ptr+=len;
		}
		/*
void skipsep(char *&ptr,size_t  &len) {
		size_t lens=0;
		do {
			len=lens;
			lens+=strspn(ptr, "	 "); 
			if(ptr[lens]=='\n') {
				newlinenr++;
				lens++;
				}
			} while(len!=lens);

		ptr+=len;
		}
		*/
#include "filter.h"

#include "myalloc.h"
int getuserlabel(ScreenPart *part, char *&ptr) {
	size_t len=0;
	skipsep(ptr,len);
	if(*ptr!='\"') {
		return -1;
		}
	ptr++;
	len=strcspn(ptr, "\""); 
	if(ptr[len]!='\"') {
		return -3;
		};
	if(!len) {
		ptr++;
		return 0;
		};
	part->userlabel = myallocar(char,len+1);
	memcpy(part->userlabel,ptr,len);
	part->userlabel[len]='\0';
	strcpyn(part->label,part->userlabel,maxlabel);
	ptr+=(len+1);
	return 0;
	}
int readfiltersfrom(ScreenPart *part,char *&ptr,char *end,char *mes) {
	char *endptr, *name;
	size_t len=0;
	int fbytes,sbytes;
	skipsep(ptr,len);
	if(*ptr!='['){
		errormessage("Miss [");
		return -1;
		}
	ptr++;	
	skipsep(ptr,len);
	while(*ptr!=']') {
		len=strcspn(ptr, "	 (\012\015"); 
//		Filter *(*func)(int,int,char*,ScreenPart*);
Filterprocptr func;
		name=ptr;
		if((func=getfilterfunc(ptr,len))) {
			ptr+=len;
			skipsep(ptr,len);
			if(*ptr!='(') {
				errormessage("Mis (");
				return -2;
				}
			ptr++;
			skipsep(ptr,len);
			len=strcspn(ptr, "	 \012\015,"); 
			fbytes=strtol(ptr, &endptr, 10); 
			ptr+=len;		
			skipsep(ptr,len);
			if(*ptr!=',') {
				errormessage("Mis , after filebytes");
				return -3;
				}
			ptr++;
			skipsep(ptr,len);
			len=strcspn(ptr, "	 \012\015,"); 
			sbytes=strtol(ptr, &endptr, 10); 
			ptr+=len;		
			skipsep(ptr,len);
			if(*ptr!=',') {
				errormessage("Mis , after screenbytes");
				return -3;
				}


			ptr++;
			skipsep(ptr,len);
			if(*ptr!='\"') {
				errormessage("Mis \" after screenbytes");
				return -3;
				}
			ptr++;
			len=strcspn(ptr, "\""); 
			if(ptr[len]!='\"') {
				errormessage("Mis \" after screenbytes");
				return -3;
				}
			ptr[len]='\0';
			Filter *fil=(*func)(fbytes,sbytes,ptr,part);
			if(!fil) {
				errormessage("Error in creatin func %s",name); 
				return -6;
				}
			if(fil->error()) {
				delete fil;
				errormessage("Error in creatin func %s",name); 
				return -5;
				}
			part->setfilter(fil);
			ptr+=len+1;
			skipsep(ptr,len);
			if(*ptr!=')') {
				errormessage("Mis )");
				return -4;
				}
			ptr++;
			skipsep(ptr,len);
			if(*ptr==',') {
				ptr++;
				skipsep(ptr,len);
				}
			}
		else {
	//		ptr+=len;
	//		skipsep(ptr,len);
			name[len]='\0';
			errormessage("Don't know filter %s",name);
			return -1;
			}
		}	
	ptr++;
	return 0;
	}
ScreenPart *str2datatype(Editor *edit,char *&ptr, char *end,char *mes) ;
char *readhasub(Editor *edit,ScreenPart *sub,char *ptr,char *end,char *mes) {
size_t len=0;
char *endptr;
			int nr=strtol(ptr, &endptr, 10); 
			if(ptr==endptr) {
				delete sub;
				errormessage("Missing element count");return NULL;
				}
			ptr=endptr;
			skipsep(ptr,len);
			if(*ptr!=',') {
					errormessage("Missing , %s",ptr);
					delete sub;
					return NULL;
					}
			ptr++; skipsep(ptr,len);
			if(*ptr!='[') {
					errormessage("Missing [ %s" ,ptr);
					delete sub;
					return NULL;
					}
			ptr++; skipsep(ptr,len);
			if(has_super(sub,Repeat)) {
				Repeat *rep=(Repeat *)sub;
				rep->part=str2datatype(edit,ptr,end,mes);
				if(!rep->part) {
					delete sub; return NULL;
					}

				rep->nrsubparts=nr;
				skipsep(ptr,len);
				}
			else {
				if(has_super(sub,Contains)) {
					Contains *con=(Contains *)sub;
					con->nrsubparts=0;
					for(int i=0;i<nr;i++) {
						ScreenPart *part=str2datatype(edit,ptr,end,mes);
						if(!part) {
							delete sub;return NULL;
							}

						skipsep(ptr,len);
						if(*ptr!=',') {
							if(i!=(nr-1)) {

								errormessage("Miss field %d of %d, get %s",i+1,nr,ptr);
								delete sub;
								delete part;
								return NULL;
								}
								}
						else {
							ptr++;
							skipsep(ptr,len);
							}
						if(con->addsubpart(part) <0) {
							delete sub;delete part;
							errormessage("Adding to composed failed");return NULL;
							}
						}
					con->nrsubparts=nr;
					}
					}
skipsep(ptr,len);
	if(*ptr!=']') {
			delete sub;
			errormessage("Missing ]");return NULL;
			}
	ptr++;skipsep(ptr,len);
		if(*ptr!=','){
			errormessage("Missing , after children %s",ptr);
			return NULL;
			}
		ptr++;
		skipsep(ptr,len);
	return ptr;
	}
extern const char datatypefile[];
ScreenPart *str2datatype(Editor *edit,char *&ptr, char *end,char *mes) {
size_t len=0;
ScreenPart *(*func)(Editor *ed);
do {
	skipsep(ptr,len);
	len=strcspn(ptr, "	 (\012\015"); 
	if((func=datatypes::getfunc(ptr,(int)len))) {
		char *token=ptr,*endptr;
		ptr+=len;		
		skipsep(ptr,len);
		if(*ptr!='('){
			errormessage("Missing ( after %s",token);
			return NULL;
			}

		ptr++;	
		skipsep(ptr,len);
		token=ptr;
		len=strcspn(ptr, "	 \012\015,"); 
		int byt=strtol(ptr, &endptr, 10); 
		ptr+=len;		
		skipsep(ptr,len);
		if(*ptr!=','){
			errormessage("Missing , after %s",token);
			return NULL;
			}
		ptr++;	
		skipsep(ptr,len);
		token=ptr;
		len=strcspn(ptr, "	 \012\015,"); 
		int fbyt=strtol(ptr, &endptr, 10); 
		ptr+=len;		
		skipsep(ptr,len);
		if(*ptr!=','){
			errormessage("Missing , after %s",token);
			return NULL;
			}

		ptr++;	
		skipsep(ptr,len);
		token=ptr;
		len=strcspn(ptr, "	 \012\015,"); 
		int bas=strtol(ptr, &endptr, 10); 
		ptr+=len;		
		skipsep(ptr,len);
		if(*ptr!=','){
			errormessage("Missing , after %s",token);
			return NULL;
			}
		ScreenPart *sub=func(edit);
		if(!sub) {
			errormessage("Creating sub datatype failed");
			return NULL;
			}
		sub->chbytes(byt);
		sub->chbase(bas);
		ptr++;	
		skipsep(ptr,len);


//		ptr++; skipsep(ptr,len);
		if(has_super(sub,HasSub)) 
			if(!(ptr=readhasub(edit,sub,ptr,end,mes)))
				return NULL;

		skipsep(ptr,len);
		if(*ptr!='\"') {
			errormessage("Mis \" before after , ");
			return NULL;
			}
		ptr++;
		len=strcspn(ptr, "\""); 
		if(ptr[len]!='\"') {
			errormessage("Mis second \" ");
			return NULL;
			}
		ptr[len]='\0';
		sub->setconvstr(ptr);
		ptr+=len+1;		
		skipsep(ptr,len);
		if(*ptr!=','){
			errormessage("Missing , after convstr");
			return NULL;
			}
		ptr++;
		skipsep(ptr,len);
		if(readfiltersfrom(sub,ptr,end,mes)<0) {
			delete sub;
			return NULL;
			}
		skipsep(ptr,len);
		if(*ptr==',') {
				ptr++;
				skipsep(ptr,len);
				if(getuserlabel(sub,ptr)<0) {
					delete sub;
					errormessage("Wrong label format");
					return NULL;
					}
				skipsep(ptr,len);
			if(*ptr==',') {
				ptr++;
				skipsep(ptr,len);
				int apart=strtol(ptr, &endptr, 10); 
				if(ptr!=endptr)
					sub->apart=apart;
				ptr=endptr;
				skipsep(ptr,len);
				if(*ptr==',') {
					ptr++; skipsep(ptr,len);
					int spaceafter=strtol(ptr, &endptr, 10); 
					if(ptr!=endptr)
						sub->spaceafter=spaceafter;
					ptr=endptr;
					skipsep(ptr,len);
					}
				if(*ptr==',') {
					}
				}
		
			}
		if(*ptr!=')') {
			errormessage("Missing )");
			delete sub;
			return NULL;;
			}
		ptr++;
		skipsep(ptr,len);
		if(sub->calcsizes()<=0) {
			delete sub;
			return NULL;
			}
		return sub;
		}
	else {
		if(memcmp(datatypefile,ptr,len)) {
			ptr[len]='\0';
			errormessage("Unknown datatype %s",ptr);
			return NULL;
			}
		ptr+=len;		
		}
	} while(1);
return NULL;
}























