/*     Bed a Binary EDitor for Linux and for Rxvt running under Linux.       */
/*     Copyright (C) 1998  Jaap Korthals Altes <binaryeditor@gmx.com>      */
/*                                                                           */
/*     Bed is free software; you can redistribute it and/or modify           */
/*     it under the terms of the GNU General Public License as published by  */
/*     the Free Software Foundation; either version 2 of the License, or     */
/*     (at your option) any later version.                                   */
/*                                                                           */
/*     Bed is distributed in the hope that it will be useful,                */
/*     but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*     GNU General Public License for more details.                          */
/*                                                                           */
/*     You should have received a copy of the GNU General Public License     */
/*     along with bed; if not, write to the Free Software                    */
/*     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.             */
/*                                                                           */
/* Sun Dec  6 18:34:27 1998                                                  */

#include "config.h"
//#define listshow 252
#define menukey 253
#define secondescape 254
#define key_next 27,91,67
#define key_prev 27,91,68
#define key_down 27,91,66
#define key_up 27,91,65
#define key_return 13
//#define key_home 27,91,49,126
/*
#define space()
#define keydef(proc,keylist...)  keys space()		##keylist = proc
*/


keydef(deletemark,4)
keydef(addfile,15)
keydef(addnext,23,110)
keydef(addprev,23,112)
keydef(backchar,8)
keylabeldef(home,1)
keylabeldef(end,5)
keydef(closethis,27,99)
keydef(copyselect,25)
keydef(fileinfo,27,61)
keydef(firstfile,27,60)
keydef(gettopos,16)
keydef(hidewindow,27,104)
keydef(zoomwindow,27,122)
keydef(keymark,7)
keydef(lastfile,27,62)
keydef(menuresized,menukey,12)
keydef(nextmark,9)
keydef(nop,menukey,27,secondescape)
keydef(altatkey,27,secondescape)
keydef(pastselect,27,112)
keydef(prevmark,27,9)
keydef(quit,27,120)	
keydef(redo,18)
keydef(removemode,22)
keydef(repeat,27,46)
keydef(replace,27,114)
keydef(rewrite,12)
keydef(saveselect,27,83)
keydef(search,27,115)
keydef(showmarks<0>,27,108)
keydef(tomark,27,103)
keydef(tosubscreen,23,23)
keydef(undo,21)
keydef(prevpos,10)
keydef(nextpos,11)
keydef(zoommode,6)
keydef(record,20)
#undef gotowin
keydef(listfiles,27,48)
#define gotowin(nr) keydef(goto##nr, 27,48+nr)

gotowin(1)
gotowin(2)
gotowin(3)
gotowin(4)
gotowin(5)
gotowin(6)
gotowin(7)
gotowin(8)
gotowin(9)
#undef gotowin

keydef(input,13)
#ifdef NCURSES_VERSION
		keylabeldef(down,27,91,66);
		keylabeldef(up,27,91,65);
		keylabeldef(pgup,27,91,53,126);
		keylabeldef(pgdn,27,91,54,126);

keydef(beginfile,27,91,49,59,53,72) //Contr-Home
keydef(endfile,27,91,49,59,53,70) //Cntr-End
#endif
#ifdef __ANDROID__
//s/keys  \([^=]*\) = \([^ ]*\) .*/keydef(\2,\1)
//s/ /,/g
keydef(setselect,27,91,50,52,126)
keydef(beginpage,27,91,50,48,126)
keydef(endpage,27,91,50,49,126)
keydef(prevmode,27,91,49,56,126)
keydef(nextmode,27,91,49,57,126)
keydef(nextfile,27,91,49,55,126)

keylabeldef(f1,27,91,91,65)
keydef(dosave,27,91,91,66)
keydef(nextsearch,27,91,91,67)
keydef(playrecord,27,91,91,68)
keylabeldef(home,27,91,49,126)
keylabeldef(home,27,91,72)
keylabeldef(end,27,91,70)
keylabeldef(end,27,91,52,126)
keylabeldef(backspace,127)
keylabeldef(f1,27,79,80)
keydef(prevfile,27,91,91,69)
keydef(prevfile,27,91,49,53,126)
keydef(dosave,27,79,81)
keydef(nextsearch,27,79,82)
keydef(beginfile,27,91,49,59,50,82)
keydef(endfile,27,91,49,59,50,83)
keydef(middlepage,27,91,49,56,59,50,126)
keydef(scrollup,27,92)
keydef(edscholldown,27,124)
keydef(scrollitem,27,91,27,91)
keydef(scrollbackitem,27,93)
keydef(scrollbyte,27,123)
keydef(scrollbackbyte,27,125)
keydef(uplevel,27,59)
keydef(downlevel,27,58)
keydef(addprev,27,91,49,53,59,50,126)
keydef(addnext,27,91,49,55,59,50,126)
keydef(playrecord,27,79,83)



#endif
#ifdef PDCURSES
keylabeldef(home,27,196)
keylabeldef(end,27,38)
keylabeldef(up,27,193)
keylabeldef(down,27,192)
keylabeldef(left,27,194)
keylabeldef(right,27,195)
keylabeldef(pgup,27,19)
keylabeldef(pgdn,27,18)
keylabeldef(delete,27,10)
#ifdef PD24
#define otherkeys 0 
keylabeldef(nothing,otherkeys,34) 
keylabeldef(nothing,otherkeys, 35)
keylabeldef(nothing,otherkeys,36)
keylabeldef(nothing,otherkeys,37)
keylabeldef(nothing,otherkeys,38)
#endif

keylabeldef(stab,27,31)
keylabeldef(f1,27,199)
keydef(dosave,27,200)
keydef(setselect,27,210)
keydef(repeat,27,182)
keydef(nextsearch,27,201)
keydef(beginfile,27,213)
keydef(endfile,27,214)
keydef(beginpage,27,207)
keydef(endpage,27,208)
keydef(middlepage,27,217)
keydef(scrollup,27,206)
keydef(edscholldown,124)
keydef(scrollitem,27,177)
keydef(scrollbackitem,27,178)
keydef(scrollbyte,123)
keydef(scrollbackbyte,125)
keydef(prevmark,27,31)
keydef(prevmode,27,205)
keydef(nextmode,27,206)
keydef(uplevel,27,179)
keydef(downlevel,58,58)
keydef(addprev,27,215)
keydef(addnext,27,216)
keydef(prevfile,27,203)
keydef(nextfile,27,204)
keydef(listfiles,27,87)
keydef(playrecord,27,202)
#else
keydef(prevmark,27,91,90)
keydef(nextchar,key_next)
keydef(backchar,key_prev)
keydef(linedown,key_down)
keydef(nextmenuitem,menukey,key_down)
keydef(lineup,key_up)
keydef(prevmenuitem,menukey,key_up)
//keydef(firstmenuitem,menukey,key_home)
keydef(nextmenu,menukey,key_next)
keydef(prevmenu,menukey,key_prev)
#ifdef __FreeBSD__
keylabeldef(backspace,127 )
keydef(nextsearch,27,91,49,51,126)
keydef(dosave,27,91,49,50,126)
keylabeldef(end,27,91,56,126)
keylabeldef(home,27,91,55,126)
keydef(beginfile,27,91,50,53,126)
keydef(endfile,27,91,50,54,126)
keydef(middlepage,27,91,51,49,126)
keydef(addprev,27,91,50,56,126)
keydef(addnext,27,91,50,57,126)
keydef(playrecord,27,91,49,52,126  )
#else
#ifdef _WIN32
keylabeldef(home,27,91,49,126 )
keylabeldef(end,27,91,52,126 )

keydef(beginfile ,27,91,50,53,126 )
keydef(endfile ,27,91,50,54,126 )
keydef(middlepage ,27,91,51,49,126 )
keydef(scrollup ,27,92 )
keydef(edscholldown ,27,124 )

keydef(scrollbyte ,27,123 )
keydef(scrollbackbyte ,27,125 )
keydef(scrollitem ,27,91,27,91 )
keydef(scrollbackitem ,27,93 )
keydef(uplevel ,27,59 )
keydef(downlevel ,27,58 )
keydef(addprev ,27,91,50,56,126 )
keydef(addnext ,27,91,50,57,126 )
#endif
#endif
#endif
//s/keydef(]*\)=[ 	]*\(.*\)/keydef(\2,\1),[ 	]*\([^)
//s/\([0-9][0-9]*\)[ 	][ 	]*\([0-9][0-9]*\)/\1,\2/g
