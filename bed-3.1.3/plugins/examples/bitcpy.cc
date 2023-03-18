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
#include <string.h>
//#include <stdio.h>
//#define DEBUG(...) fprintf(stderr,__VA_ARGS__)
#define DEBUG(form...)
int bitcpy(unsigned char *output,const unsigned char *const input,const char bitout,const char bitin,const int bits) {
	short lastout=(bitout+bits)/8;
	short bitsover=(bitout+bits)%8;
	unsigned short last=(((*((unsigned short *)(output+lastout)))>>bitsover)&0xffff)<<bitsover;
	if(bitout==bitin) 
		{
		unsigned char last=((output[lastout]>>bitsover)&0xff)<<bitsover;
		*output=((*output<<(8-bitout))&0xff)>>(8-bitout)|((((*input)>>bitout)&0xff)<<bitout);
		if(lastout>0)
			memmove(output+1,input+1,lastout);
		output[lastout]=	(((output[lastout]<<(8-bitsover))&0xff)>>(8-bitsover))|last;
 

		}
	else {
		unsigned char first=*output;
		memset(output+1,'\0',lastout);
		*output=(((first<<(8-bitout))&0xff)>>(8-bitout));
	if(bitout>bitin) {
		int shift=bitout-bitin;
		for(int i=lastout;i>=0;i--) {

			*((unsigned short *)(output+i))|=(*((unsigned short *)(input+i))<<shift);
			}
		output[0]=(((first<<(8-bitout))&0xff)>>(8-bitout))| (((output[0]>>bitout)&0xff)<<bitout);
		output[lastout]=	(((output[lastout]<<(8-bitsover))&0xff)>>(8-bitsover))|last;
		output[lastout+1]=((char *)&last)[1];

		}
	else
	if(bitout<bitin) {
		int shift=bitin-bitout;
		for(int i=lastout;i>=0;i--) {

			*((unsigned short *)(output+i))|=(*((unsigned short *)(input+i))>>shift);
			}
		output[0]=(((first<<(8-bitout))&0xff)>>(8-bitout))| (((output[0]>>bitout)&0xff)<<bitout);
		output[lastout]=	(((output[lastout]<<(8-bitsover))&0xff)>>(8-bitsover))|last;
		output[lastout+1]=((char *)&last)[1];
 

		}
	}	
	return 0;
	}
