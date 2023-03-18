#include <stdio.h>
#include <ctype.h>
//#define __GNU_VISIBLE 1
#define _GNU_SOURCE     1
#include <string.h>
char start='#';
#define prodef(x) #x,
#define prodefname(name,pr) #name,
#include "getlabel.h"
const char *procnames[]={

#include "procedures.h"
};
#undef prodef
#undef prodefname
#define prodef(x) NULL,
#define prodefname(name,pr) #pr,
const char *procproc[]={

#include "procedures.h"
};
int procnamesnr=sizeof(procnames)	/sizeof(procnames[0]);

//#define readaline(buf) gets(buf) /*I leave it gets, \n should be removed*/

////////////////#define readaline(buf) (scanf("%4095[^\n]s", buf)>0?buf:NULL) /* \n should be removed*/
//#define getliner(buf) scanf("%4095[^\n]s", buf) /* \n should be removed*/

#define getliner(buf) fgets(buf,sizeof(buf),stdin)


int main(void) {
	char buf[4096],uit[5000];
	char *getret=NULL;
	FILE *names,*indexs,*help;
	do {
		getret= getliner(buf);
		if(!getret) {
			fprintf(stderr,"no %c\n",start);
			return -1;
			}
		} while(buf[0]!=start);
	names=fopen("helptext.h","w");	
	indexs=fopen("helpindex.h","w");	
	help=fopen("helpdata.h","w");	
	do {
		char *name=buf+1;
		int ind[100],nr=0;
		int startpos,i,iline,procedure;

#ifdef HAVE_STRCHRNUL
		*strchrnul(name,'\n')='\0'; /*Ok fgets instead of gets*/
#else
	{
	;
	if(char *ptr=strchr(name,'\n'))	
			*ptr='\0'; /*Ok fgets instead of gets*/
	}
#endif
		procedure=getlabel(procnames,procnamesnr,name);
		if(procedure>=0) {
			if(procproc[procedure])
				fprintf(help,"{helpprocname(%s,%s),",name,procproc[procedure]);
			else
				fprintf(help,"{helpname(%s),",name);
			 fprintf(names,"char %sname[]=\"",name);
			 fprintf(indexs,"short %sind[]={",name);
			}

		getret= getliner(buf);
		if(!getret) {
			fprintf(stderr,"no text");
			return -1;
			}
		iline=0;
		do  {
		if(procedure>=0) {
		  int escps=0,u=0;
	   	   startpos=ftell(names);
		   for(int j=0;buf[j];j++) {
			   if(buf[j]=='\\'||buf[j]=='\"'){
				   uit[u++]='\\';
				   escps++;
				  }
			else {
				if(buf[j]=='\n')		/*OK fgets instead of gets*/
					break;
				}

			   uit[u++]=buf[j];
		   	}
		uit[u]='\0';
		   fprintf(names,"%s\\000",uit);
		  // fputc(0,names);
		   iline=ind[nr]=iline+u+1-escps;
		   nr++;
		   	}
			getret=getliner(buf);
		   	} while(getret&&buf[0]!=start);
		if(procedure>=0) {
		 fprintf(help,"%d},\n",nr);
		 fprintf(names,"\";\n");
		 for(i=0;i<(nr-1);i++) {
			 fprintf(indexs,"%d,",ind[i]);
		 	}
		 fprintf(indexs,"};\n");
		 	}
		} while(getret); 
	fclose(names);
	fclose(help);
	fclose(indexs);
	return 0;
	}



