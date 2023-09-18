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

#include <features.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "digitstring.h"
#include "myalloc.h"
#define maxnum( x,y) ((x>y)?x:y)
#define  verleng( line, mint, len) 
/*
void verleng(int line,struct mint *m,int len) {
			fprintf(stderr,"%d: Verleng %s, %d\n"	,line,m->val,len);
//		m->max=len;
//		m->val=(unsigned char *)realloc(m->val,len);
		}
*/	
/*uit=in1-in2 */
int subtract(struct mint *uit,const struct mint *in1,const struct mint *in2) {
	int i,res,uitmax=uit->max;
	uit->val[0]=0;
	for(i=0;i<in2->len&&i<uitmax;i++) {
		int nexti=i+1;
		if(uitmax>nexti)
			uit->val[nexti]=0;
		res=(in1->val[i]-in2->val[i])-uit->val[i];
		while(res<0)	{
			res+=256;
			if(uit->max>nexti)
				uit->val[nexti]++;
			}
		uit->val[i]=res;
		}
	for(;i<in1->len&&i<uitmax;i++) {
		int nexti=i+1;
		if(uitmax>nexti)
			uit->val[nexti]=0;
		res=in1->val[i]-uit->val[i];
		while(res<0)	{
			res+=256;
			if(uitmax>nexti)
				uit->val[nexti]++;
			}
		uit->val[i]=res;
		}
	uit->len=i;	
	return 0;
	}
int add(struct mint *uit,const struct mint *in1,const struct mint *in2) {
	int maxlen=maxnum(in1->len,in2->len),res=0,i;
	if(maxlen>uit->max) {
		verleng(__LINE__,uit,uit->max*2);
		return 1;
		}
	for(i=0;i<maxlen;i++) {
		res+=((i<in1->len)?in1->val[i]:0)+((i<in2->len)?in2->val[i]:0);
		uit->val[i]=res%256;
		res/=256;
		}
	while(	res>0) {
		if(i>=uit->max) {
			verleng(__LINE__,uit,uit->max*2);
			return 1;
			}
		uit->val[i++]=res%256;
		res/=256;
		}
	uit->len=i;
	return 0;
	}
int divide(struct mint *uit,const struct mint *in,const int over) {
		int i,nonz=0;
		div_t res;
		res.rem=0;
		for(i=in->len-1;i>=0;i--) {
			res=div(in->val[i]+(res.rem*256),over);
			uit->val[i]=res.quot;
			if(!nonz&&res.quot)
				nonz=i+1;
			}
		uit->len=nonz;
		return res.rem;
		}
int multi(struct mint *uit,const struct mint *in,int keer) {
	int i;
	unsigned long long res=0;
	if(in->len>uit->max) {
		verleng(__LINE__,uit,uit->max*2);
		return 1;
		}
	for(i=0;i<in->len;i++) {
		res+=in->val[i]*keer;
		uit->val[i]=res%256;
		res/=256;
		}
	while(	res>0) {
		if(i>=uit->max) {
			verleng(__LINE__,uit,uit->max*2);
			return 1;
			}
		uit->val[i++]=res%256;
		res/=256;
		}
	uit->len=i;
	return 0;
	}
/*
int todigitstring(struct mint *in,char *res) {
	char rem[80];
	int len=79,i,sel;
	struct mint uit[2];
	uit[0].len=uit[1].len=in->len;
	uit[0].max=uit[1].max=in->len+1;
	uit[0].val=myallocarp(unsigned char *,char,uit[0].max);
	uit[1].val=myallocarp(unsigned char *,char,uit[0].max);
	memmove(uit[1].val,in->val,in->len);
	rem[len]='\0';	
	sel=1;
	while(1) {
		again:
		rem[--len]='0'+divide(&uit[!sel],&uit[sel],10);
		sel=!sel;
		for(i=0;i<uit[sel].len;i++)
			if(uit[sel].val[i])
				goto again;
		break;
		}
	memmove(res,rem+len,80-len);
	myfree(uit[0].val);
	myfree(uit[1].val);
	return 0;
	}
*/
/*
#define todigit(get) (get<10)?'0'+get:get+('A'-10)
#define fromdigit(ch) (ch>'9')?get+('A'-10)
*/
#include "convert.h"
#define todigit(get) hexel(get)
#define fromdigit(ch) chars2dig(ch)

int todigitstring(struct mint *in,int  base,char *rem,int len) {
	int i,sel;
	struct mint uit[2];
	uit[0].len=uit[1].len=in->len;
	uit[0].max=uit[1].max=in->len+10;
	uit[0].val=myallocarp(unsigned char *,char,uit[0].max);
	uit[1].val=myallocarp(unsigned char *,char,uit[0].max);
	memmove(uit[1].val,in->val,in->len);
	rem[len]='\0';
	memset(rem,'0',len);
	sel=1;
	while(1) {
		again:
		rem[--len]=todigit(divide(&uit[!sel],&uit[sel],base));
		sel=!sel;
		for(i=0;i<uit[sel].len;i++)
			if(uit[sel].val[i])
				goto again;
		break;
		}
	myfree(uit[0].val);
	myfree(uit[1].val);
	return 0;
	}
/*
int todigitstring(struct mint *in,char *res) {
	return todigitstring(in,10,res,strlen) ;
	}
*/
int fromdigitstring(const char *dec,int declen,int base,struct mint *uit) {
	int i;
	struct mint power,tijd;
	tijd.max=uit->max+10;
	tijd.val=myallocarp(unsigned char *,char,tijd.max);
	tijd.len=0;
	power.max=uit->max+10;
	power.val=myallocarp(unsigned char *,char,power.max);
	power.val[0]=1;
	power.len=1;
	uit->len=0;
	for(i=declen-1;i>=0;i--) {
		int dig=fromdigit(dec[i]);
		if(dig<0) {
			myfree(tijd.val);
			myfree(power.val);
			return -1;
			}
		if(multi(&tijd,&power,dig)||add(uit,uit,&tijd)||multi(&power,&power,base)) { 
			myfree(tijd.val);
			myfree(power.val);
			return -2;
			}
		}
	myfree(tijd.val);
	myfree(power.val);
	return 0;
	}
#undef TEST
#ifdef TEST
int fromdigitstring(char *dec,struct mint *uit) {
	 return fromdigitstring(dec,strlen(dec),10,uit) ;
	}
#if 0
#define LONGLONG long long
#define MAXNUM sizeof(LONGLONG)
int main(void) {
unsigned long i;
	struct mint in1,in2,in3;	
	LONGLONG get1,get2,get3;
	in1.val=(unsigned char*)alloca(MAXNUM);in1.max=MAXNUM;
	in2.val=(unsigned char*)alloca(MAXNUM);in2.max=MAXNUM;
	in3.val=(unsigned char*)alloca(MAXNUM);in3.max=MAXNUM;
srand(time(NULL));
//randomize();
for(i=0;i<0xFFFFFFFF;i++) {
	*((LONGLONG *)in3.val)=0LL;
	get1=*((LONGLONG *)in1.val)=rand()%INT_MAX;
	in1.len=MAXNUM;
	get2=*((LONGLONG *)in2.val)=rand()%INT_MAX;
	in2.len=MAXNUM;
	subtract(&in3,&in1,&in2);
	get3=get1-get2;
	if(get3!=*((LONGLONG *)in3.val)) {
		printf("%lld-%lld=%lld (they) or %ld(me)\n",get1,get2,get3, *((LONGLONG *)in3.val));
		}
	}
return 0;
}
#else

#if 0
#define MAXNUM 48000
#define MAXSTR 512
int todigitstring(struct mint *in,char *res) {
	return todigitstring(in,10,res,MAXSTR) ;
	}
int main(void) {
	struct mint in1,in2,in3;	
	long get1,get2,get3;
	in1.val=(unsigned char*)alloca(MAXNUM);in1.max=MAXNUM;
	in2.val=(unsigned char*)alloca(MAXNUM);in2.max=MAXNUM;
	in3.val=(unsigned char*)alloca(MAXNUM);in3.max=MAXNUM;
	char str1[MAXSTR],str2[MAXSTR],str3[MAXSTR];	
	while(1) {
memset(in1.val,'\0',4);
memset(in2.val,'\0',4);
memset(in3.val,'\0',4);
		printf("one: ");
		scanf("%s", str1);
		fromdigitstring(str1,&in1);
		todigitstring(&in1,str3);
		puts(str3);
		printf("two: ");
		scanf("%s", str2);
		fromdigitstring(str2,&in2);
		todigitstring(&in2,str3);
		puts(str3);
		subtract(&in3,&in1,&in2);
		todigitstring(&in3,str3);
		printf("%s-%s=%s\n",str1,str2,str3);
		get1=*((long *)in1.val); 
		get2=*((long *)in2.val); 
		get3 =*((long *)in3.val);
		printf("%ld-%ld=%ld (%ld)\n",get1,get2,get3,get1-get2);
		}
return 0;
	}
#else
int main(void) {
	struct mint in1,in2,uit;	
	char res[256];
	unsigned int hex,back;
	scanf("%X",&hex);
	uit.val=myallocarp(unsigned char *,char,200);
	memmove(uit.val,(unsigned char *)&hex,4);
	in1.val=myallocarp(unsigned char *,char,200);
	in1.max=200;
	in1.len=0;
	uit.max=200;
	uit.len=4;

//	todigitstring(&uit,res,10);
	todigitstring(&uit,res);
	fromdigitstring(res,&in1);
	memmove((unsigned char *)&back,in1.val,4);
	printf("Original: %X, digit %s,back: %X\n",hex,res,back);
/*
	char res[80],
	char str1[256],str2[256];
	int get,i;
	uit.val=(char *)malloc(200);
	uit.max=200;
	uit.len=0;
	in1.val=(char *)malloc(200);
	in1.max=200;
	in1.len=0;
	in2.val=(char *)malloc(200);
	in2.max=200;
	in2.len=0;
	while(1) {
		printf("Getal: ");
		scanf("%s", str1);
		fromdigitstring(str1,&in1);
		todigitstring(&in1,res);
		printf("%s became %s\n",str1,res);	
		printf("vermenig: ");
		scanf("%s", &str2);
		fromdigitstring(str2,&in2);
		todigitstring(&in2,res);
		printf("%s became %s\n",str2,res);	
		uit.len=0;
		add(&uit,&in1,&in2);
		todigitstring(&uit,res);
		printf("todigitstring %s\n",res);
		}
	return 0;
*/
	}
#endif
#endif

#endif
#ifdef TEST
int main(void) {
struct mint in;	
unsigned char *buf=in.val=(unsigned char*)alloca(10);in.max=4;
in.len=0;
memset(in.val,'\0',10);
fromdigitstring("99999 ",5,10,&in);
unsigned short *get=(unsigned short *)buf;
printf("%hu\n%hu\n",get[0],get[1]);
return 0;
}
#endif
