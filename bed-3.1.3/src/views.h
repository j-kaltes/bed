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
#ifdef  __cplusplus
extern "C" {
#endif
const char *mvtakefile(int ypos,int xpos,const char *com,const char *pat) ;
const char *takefile(const char *com,const char *pat) ;
void getfilecolors(void) ;


char * selectitem(const char *prompt,char **files,int nr,int start) ;

extern int  selectitemindex(const char *prompt,char **files,int nr,int start,int col=60,int line=15) ;
//int  selectitemindex(char *prompt,char **files,int nr,int start) ;
#ifdef  __cplusplus
}
extern int  selectitemindexhelp(const char *prompt,char **files,int nr,int start,int &givehelp,int c=60,int l=15) ;
//int  selectitemindex(char *prompt,char **files,int nr,int start,int &givehelp) ;
#endif
