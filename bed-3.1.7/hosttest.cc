
#include <signal.h>
#include <setjmp.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include "src/confdefines.h"
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

int printzd(FILE *defs) {

	ssize_t get= 12345;
	int getin=0;
	const char testfile[]= "conftest.printzd";
	FILE *file=fopen(testfile,"w");
	if(!file)
		return(2);
	fprintf(file,"%zd\n",get);
	fclose(file);
	file=fopen("conftest.printzd","r");
	fscanf(file,"%d",&getin);
	fclose(file);
	if(getin==get) {
		fputs("#define PRI_SIZE_T_MODIFIER \"z\"\n" ,defs);
//		fputs("#define HAS_PRINTF_zd 1\n" ,defs);
		}
	else
		fputs("/*#undef PRI_SIZE_T_MODIFIER*/\n" ,defs);
	unlink(testfile);
	return(0);
}

#ifdef HAS_SIGSETJMP
static sigjmp_buf  testjmpbuf;
void sighand(int sig) {
	signal(SIGINT,sighand);
	siglongjmp(testjmpbuf, 4);
	}
int jumper(FILE *defs) {
	volatile int ret=0;
	sigset_t set;
	fprintf(stderr,"Checking for longjump from interrupt more than once... ");
	fflush(stderr);
	pid_t pid=getpid();
	signal(SIGINT,sighand);
	if(sigsetjmp(testjmpbuf,1)==4) {
		putchar('i');
		ret++;
		if(ret>4) {
			puts("yes");
			fputs("#define NOUNBLOCKSIGNALS 1\n" ,defs);
			return( 0);
			}
		}
	kill(pid,SIGINT);
	puts("no");
	
	fputs("/*#undef NOUNBLOCKSIGNALS */\n" ,defs);
	return( 4);
	}
#else
#define jumper(defs) 
#endif
int pidfile(FILE *defs,const char * const procform) {
static  pid_t pid=getpid();
char procname[50];
if(snprintf(procname,50,procform,pid)>0&&access(procname,F_OK)==0)  {
	fprintf(defs,"#define PROCEXEFILENAMEFORMAT \"%s\"\n",procform);
	return 1;
	}
return 0 ;
}

#ifndef OWNTRANSTIME 
extern int retime(FILE *defs)  ;
#else
#define retime(_x) 
#endif


int checkmeminfo(void) {
        char gegs[80];
        FILE *fp;
        int memfree=-1,buffers=-1,cached=-1;
        long long freem;
        if(!(fp=fopen("/proc/meminfo","r"))) {
                return -1;
                }
        while(fgets(gegs,80,fp))  {
                if(!strncmp("MemFree:",gegs,8)) {
                        sscanf(gegs+9,"%d",&memfree);
                        }
                if(!strncmp("Buffers:",gegs,8)) {
                        sscanf(gegs+9,"%d",&buffers);
                        }
                if(!strncmp("Cached:",gegs,7)) {
                        sscanf(gegs+8,"%d",&cached);
                        if(memfree>0&&buffers>=0)
                                break;
                        }
                }
        fclose(fp);
        if(memfree==-1||buffers==-1||cached==-1) 
		return -2;
        return 0;
        }

void meminfo(FILE *defs) {
	if(checkmeminfo()==0)  {
		fputs("#define HAS_MEMINFO 1\n",defs);
		}
else {
	fputs("/*#undef HAS_MEMINFO*/\n" ,defs);

}
}


#if  SIZEOF_LONG_DOUBLE == 16
/*
static int printlb(long double *l,char uit[],int len) {
return snprintf(uit,len,"%.*Lg\n",1000,*l);
}
*/

void floattest(FILE *defs) {
unsigned int get1[4]= {0xFFFFFFF5,0xFFFFFFFF,0x0FFFF0FF,0x0};
unsigned int get2[4]= {0xFFFFFFF5,0xFFFFFFFF,0x0FFFF0FF,0xFFAA1234};
long double *l1=(long double *) get1;
long double *l2=(long double *) get2;

fprintf(stderr,"Checking long double sizeof(long double)=%zu uses only 12 bytes... ",sizeof(long double));
if(*l1!=*l2) {
	fputs("No\n",stderr);
	fputs("#define REALLONGDOUBLE16\n",defs);
	return;
	}
	
fprintf(stderr,"Yes: %.*Lg==%.*Lg\n",20,*l1,20,*l2) ;
fputs("#undef REALLONGDOUBLE16\n",defs);
return;
}
#else
#define floattest(defs) 
#endif

//#define process(commando,...)    (fork()?(wait(&statusdieprocessreturns),statusdieprocessreturns):( (execlp(commando,commando,__VA_ARGS__,NULL), perror(commando)), exit(4),1))

#ifdef HAVE_SYS_WAIT_H
char file[]="file";
int testfile(char *const argv[]) {
int status;
if(fork()) {
	return (wait(&status)<0)|| !WIFEXITED(status)|| WEXITSTATUS(status);

	}
else {
	execvp(file,argv);
	perror(file);
	exit(4);
	}
			
}
void testfile(FILE *defs,char *prog) {
{
char *argv[3]={file,prog,NULL};
if(!testfile(argv)) 
	fputs("#define HAS_FILE 1\n",defs);
else
	fputs("/*#undef HAS_FILE */\n",defs);
	
};
char opt[]="-b";
char *argv2[]={file,opt,prog,NULL};
if(!testfile(argv2)) 
	fputs("#define BRIEF_FILE 1\n",defs);
else
	fputs("/*#undef BRIEF_FILE */\n",defs);
opt[1]='z';
if(!testfile(argv2)) 
	fputs("#define UNCOMPRESS_FILE 1\n",defs);
else
	fputs("/*#undef UNCOMPRESS_FILE */\n",defs);
	
}
#else
#define testfile(x,y) 
#endif
int main(int argc,char **argv) {
FILE *defs=fopen(HOSTCONF,"w");
if(!defs) {
	perror(HOSTCONF);	
	return 2;
	}

if (access("/usr/share/applications", F_OK)==0)
	fputs("#define HAS_APPLICATIONS \"/usr/share/applications\"\n",defs);

if(!pidfile(defs,"/proc/%d/file"))
	pidfile(defs,"/proc/%d/exe");
testfile(defs,argv[0]);
meminfo(defs);
retime(defs);
printzd(defs);
floattest(defs);
fflush(defs);
jumper(defs);
fclose(defs);
}
