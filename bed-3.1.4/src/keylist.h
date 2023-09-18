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


// char *keysyms[]= { "backspace", "delete", "down", "end", "esc", "home", "left", "pgdn", "pgup", "right", "stab", "up" };
//keylabel(label,edit,menu,open)
keylabel("backspace",funcptr(Editor,backchar),NULL,BACKSPACE)
keylabel("delete",NULL,NULL,DELETEKEY)
keylabel("down",funcptr(Editor,linedown),funcptr(Editor,nextmenuitem),DOWN)
keylabel("end",funcptr(Editor,endline),funcptr(Editor,lastmenuitem),END)
keylabel("esc",funcptr(Editor,altatkey),funcptr(Editor,nop),QUIT)
keylabel("f1",funcptr(Editor,moveshow),funcptr(Editor,showhelp),HELP)
keylabel("home",funcptr(Editor,beginline),funcptr(Editor,firstmenuitem),BEG)
keylabel("left",funcptr(Editor,backchar),funcptr(Editor,prevmenu),PREV)
keylabel("nothing",funcptr(Editor,nop),funcptr(Editor,staymenu),NOP)
keylabel("pgdn",funcptr(Editor,pagedown),NULL,PGDN)
keylabel("pgup",funcptr(Editor,pageup),NULL,PGUP)
keylabel("right",funcptr(Editor,nextchar),funcptr(Editor,nextmenu),NEXT)
keylabel("stab",funcptr(Editor,prevmark),NULL,SHIFTAB)
keylabel("up",funcptr(Editor,lineup),funcptr(Editor,prevmenuitem),UP)

