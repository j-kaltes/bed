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
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
       #include <sys/types.h>
       #include <sys/stat.h>
       #include <unistd.h>



int main(int argc, char **argv) {
	FILE *infp,*uitfp;
	int ch;
	if(argc<4) {
		fprintf(stderr,"Gebruik %s infile identifier uitfile\n",argv[0]);
		fprintf(stderr,"Maakt character array van gegevens in <infile> onder naam <identifier> en stopt dat in uitfile\n");
		exit(1);
		};
		
	if(!(infp=fopen(argv[1],"rb"))) {
		fprintf(stderr,"Can't open %s for reading\n",argv[1]);
		exit(2);
		}
	if(!(uitfp=fopen(argv[3],"wb"))) {
		fprintf(stderr,"Can't open %s for writing\n",argv[3]);
		exit(2);
		}
	struct stat st;
	 if(fstat(fileno(infp), &st)==0) {
		int len=((argc==4)?1:0)+st.st_size;
		fprintf(uitfp,"const unsigned char %s[%d] = {",argv[2],len);
		}
	 else {
		fprintf(uitfp,"const unsigned char %s[] = {",argv[2]);
		}
	while((ch=getc(infp))!=EOF) {
		if(isalnum(ch))
			fprintf(uitfp,"\'%c\',",ch);
		else
			fprintf(uitfp,"0x%x,",ch);
		}
	if(argc==4)
		putc('0',uitfp);

	fputs("};\n",uitfp);
	return 0;
	}
