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

#ifdef GLOBAL
#include "global.h"
BEDPLUGIN

#include <stdio.h>
#include "../screenpart.h"
#include <limits.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "signed.h"
class Editor {
public:
#include "faked.h"
  int getanswer(const char *question,char *answ,int anslen,int ansroom=5) ;
};

class  Subtract : public Signed {	
	long subtract;
	public:	 
		declareparent 
	Subtract( Editor *ed,int b,int x1=0, int y1=0): Signed( ed, b, x1, y1) 	{
	subtract=0;
	};	
	Subtract( Editor *ed, int x1=0, int y1=0): Signed (ed,1,x1, y1) {
	subtract=0;
	};	

	 Subtract ( Subtract const &sig):Signed(sig) {
			subtract=sig.subtract;
			}
	int form2bin(const char *formal, unsigned char *bin, int &len) const override {	
		int res=Signed::form2bin(formal, bin,len) ;	
		switch(datbytes()) {
			case 1: *((char *)bin)+=subtract;break;
			case 2: *((short *)bin)+=subtract;break;
			case 4: *((long *)bin)+=subtract;break;
			case 8: *((long long *)bin)+=subtract;break;
			};
		return res;
		}

	ScreenPart *newone(void) const  override {
		return new Subtract(*this);
		}
	int bin2formal(unsigned char *bin, char *formal, int len) override {	
		unsigned char newbin[datbytes()];	
		switch(datbytes()) {
			case 1: *((char *)newbin)=*((char *)bin)-subtract;break;
			case 2: *((short *)newbin)=*((short *)bin)-subtract;break;
			case 4: *((long *)newbin)=*((long *)bin)-subtract;break;
			case 8: *((long long *)newbin)=*((long long *)bin)-subtract;break; 
			};
		return Signed::bin2formal(newbin,formal,len) ;	
		}
	virtual int userconfig(int kind) override {
			Signed::userconfig(kind);
			const int anslen=10;	
			char answ[anslen]="",*endp;	
			snprintf(answ,anslen,"%ld",subtract);
			editor->getanswer("Subtract: ",answ,anslen);
			subtract=strtol(answ,&endp,0);
			return 0;
			}
	};

parentd(Signed,Subtract) 
#include "../typelist.h"
datatypename(Subtract,Subtract::thename )
#endif



#ifdef EDITOR


#endif

//call(SIGNEDINT)
