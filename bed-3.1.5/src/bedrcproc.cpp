#include "defines.h"
#define leaveprocedure  return
initword(context){
			if(!ptr) {
				fprintf(stderr,"line %d: nothing after %s\015\012",l+1,gegs);
				leaveprocedure;
				}
			char *is,*val=next(ptr);
			int negate;
			if(*ptr=='!') {
				if(ptr[1]) {
					ptr++;
					}
				else {
					ptr=val;
					val=next(ptr);
					}
				negate=1;
				}
			else
				negate=0;

			if(!(is=getenv(ptr))||(val&&(toendline(val),strcmp(val,is)))) {
					outofcontext=1;
					}
			else
				outofcontext=0;
			if(negate)
				outofcontext=!outofcontext;
			leaveprocedure;
			}

initword(color){
			if(makeattr(ptr)<0) 
				fprintf(stderr,"line %d: %s\015\012",l+1,mess);
			leaveprocedure;
			}
initword(keys){
				if(!ptr) {
				fprintf(stderr,"line %d: missing argument of keys\015\012",l+1);
				}
				else
				if(makekeys(ptr)<0)
					fprintf(stderr,"line %d: %s\015\012",l+1,mess);
				}
initword(plugin){
					next(ptr);
					if(!ptr)
						fprintf(stderr,"Nothing after plugin\015\012");
					else {
						if(dynlink(ptr)<0)
							fprintf(stderr,"line %d: %s\015\012",l+1,mess);
						}
					}

initword(plugindir){
					//	next(ptr);
						toendline(ptr);
						if(!ptr) fprintf(stderr,"nothing after plugindir\015\012");
						else {
							char tmpplug[BEDSMAXPATH];
							versionabsolutefilename(tmpplug,ptr);
						plugindirlen=strlen(tmpplug)+1;
						if(plugindir!=confdir)	
							myfree( plugindir);
						plugindir=myallocar(char,plugindirlen);
						memcpy(plugindir,tmpplug,plugindirlen);
						}
						}

initword(datatypedir){
					//	next(ptr);
						toendline(ptr);
						if(!ptr) fprintf(stderr,"nothing after datatypedir\015\012");
						else {
							versionabsolutefilename(datatypedirectory,ptr);
						}
						}

initword(macrodir){
					//	next(ptr);
						toendline(ptr);
						if(!ptr) fprintf(stderr,"nothing after macrodir\015\012");
						else {
							versionabsolutefilename(macrodirectory,ptr);
						}
						}
initword(bedrcbin){
					//	next(ptr);
						toendline(ptr);
						if(!ptr) fprintf(stderr,"nothing after bedrcbin\015\012");
						else {
							versionabsolutefilename(bedrcbin,ptr);
						}
						}
initword(newfilesize){
						toendline(ptr);
						if(!ptr) fprintf(stderr,"no number after newfilesize\015\012");
						else {
							char *endptr;
							int len= strtol(ptr, &endptr, 0);
							if(endptr==ptr) 
								fprintf(stderr,"no number after newfilesize: %s\07\015\012",ptr);
							else
								if(*endptr&&!isblank(*endptr)) {
									fprintf(stderr,"newfilesize: additional characters after number: %s\07\015\012",ptr);
									}
								else {
									if(len<1) 
										fprintf(stderr,"newfilesize %d, too small\015\012",len);
									else {
										newfilesize=len;

										}

									}

							}
						}
initwordname(execute,call_on_open)
initword(call_on_open){
						next(ptr);
						if(!ptr)
							fprintf(stderr,"Miss procedure after execute\015\012");
						else
						if(str_call_open(ptr)<0) {
							fprintf(stderr,"line %d: problem adding %s\015\012",l+1,ptr);
							}
						}
initword(call_on_switch){
						next(ptr);
						if(!ptr)
							fprintf(stderr,"Miss procedure after execute\015\012");
						else {
						if(str_call_switch(ptr)<0) {
			 
							fprintf(stderr,"line %d: problem adding %s\015\012",l+1,ptr);
							}
							}
						}

initword(defaultcolor){
			if(mkground(ptr)<0)
				fprintf(stderr,"line %d: %s\015\012",l+1,mess);
			}
initword(call_on_start){
						next(ptr);
						if(!ptr) {
							fprintf(stderr,"Miss procedure after %s\015\012",gegs);leaveprocedure;
						}

				if(str_call_once(ptr)<0) {
						fprintf(stderr,"line %d: problem adding procedure %s\015\012",l+1,ptr);
						}
				}
initwordproc(defaulthandler)	{
	if(str_call_once(gegs)<0) {
		fprintf(stderr,"line %d: problem adding procedure %s\015\012",l+1,gegs);
				}
	};

initword(menu)	{
				if(!ptr) {
				fprintf(stderr,"line %d: missing argument of menu\015\012",l+1);
				}
				else
				if(makemenu(ptr)<0)
					fprintf(stderr,"line %d: %s\015\012",l+1,mess);
				}
initword(menuitem)	
{
	if(!ptr) {
	fprintf(stderr,"line %d: missing argument of menuitem\015\012",l+1);
	}
	else
	if(makemenuitem(ptr)<0)
		fprintf(stderr,"line %d: %s\015\012",l+1,mess);
	}

initword(deletemenu)	{
				if(deletemenu(ptr)<0)
					fprintf(stderr,"line %d: %s\015\012",l+1,mess);
				}

initword(deletemenuitem)	{
				if(deletemenuitem(ptr)<0)
					fprintf(stderr,"line %d: %s\015\012",l+1,mess);
				}
initword(editor)	{
	if(ptr) {
		toendline(ptr);
		setenv("EDITOR",ptr,1);
		}
	else
		unsetenv("EDITOR");
	}	
initword(setenv)	{
	if(!ptr)
		fprintf(stderr,"line %d: missing argument of setenv\015\012",l+1);
	else {
		char *val=next(ptr);
		if(val) {
			toendline(val);
			char tmp[256];
			setenv(ptr,replaceversion(tmp,val),1);
			}
		else
			unsetenv(ptr);
		}
	}	
initword(options)	{
	if(!ptr)
		fprintf(stderr,"line %d: missing argument of args\015\012",l+1);
	else {
		char *rcargv[30];
		int rcargc=0;
		do  {
			rcargv[rcargc++]=ptr;
			ptr=next(ptr); 
			} while(ptr);
	/*	
		fprintf(stderr,"args");
		for(int i=0;i<rcargc;i++) 
			fprintf(stderr," %d#%s#",i,rcargv[i]);
*/
		doargs(0,rcargc,rcargv) ;
		}
	}
