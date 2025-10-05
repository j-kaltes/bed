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
#define THESHELL elm("S~hell", shell),

#define dataproc(lab,type) elm(lab,add##type),
#define menuproc(lab,type) elm(lab,type),
#define menuprock(lab,key,type) elk(lab,key,type),
#include "dataprocer.h"
#if defined(USE_WINFILE) || defined(__linux__)
#define USE_SELECTDISK 1
#endif
#ifdef USE_SELECTDISK
#define SELECTDISK elm("~Disk",selectdisk),
#else
#define SELECTDISK 
#endif
menumake(file,"~File",
	elm("~New",emptyfile),
	elk("~Open","C-o",addfile),
	SELECTDISK
	LINE,
	elm("~Fileinfo", filedata),
	elm("~Modified?",checkchanged),
	elk("~Save","F2",dosave),
	elm("Save ~as",saveas),
	LINE,
	elk("~Refresh","C-l",rewrite),
	THESHELL
	LINE,
	elk("~Close","M-c",closethis),
	elk("E~xit","M-x",quit))



menumake(edit,"~Edit", 
	elk("~Undo","C-u",undo), 
	elk("~Redo","C-r",redo),
	LINE ,

	elk("~Select","F12",setselect), 
	elk("~Copy","C-y",copyselect), 
	elk("~Past","M-p",pastselect), 
	elm("S~ave selection",saveselect),
	LINE,
        elk("R~eplace","M-r",replace),
#ifdef PDCURSES
	elm("Repeat ~nr",repeat),
#else
	elk("Repeat ~nr","M-.",repeat),
#endif
	LINE,
	elm("E~xtend file" ,extend),
	elm("~Truncate" ,truncate), 
	LINE,
	elm("Sa~ve Shown", saveshown),
	elm("Insert s~hown" ,readshown), 
	CLIPBOARD
	elm("E~dit" ,editdata), 
	elk("~Input" ,"Enter",input),
	elm("Select Stand~out",selectstandout))
menumake(move,"~Move", 
	elk("~Search","M-s",search),
	elk( "~Nextsearch","F3",nextsearch), 
	elm( "~Last searched",gotolastsearchpos), 
	LINE,
	elk("~Position","C-p",gettopos), 
	LINE,
	EXTRAMOVE 
	elk("~Begin file", "S-F3",beginfile),
	elk("~End file", "S-F4",endfile),
	elk("Beg~in Pg","F9",beginpage),
	elk("En~d Pg","F10",endpage),
	elk("Middle~ Pg","S-F7",middlepage), 
	LINE,
	elk("Undo ~move", "C-j",prevpos),
	elk("Redo mo~ve", "C-k",nextpos))

menumake(shift,"Sh~ift", 
	elm("~Top",postotop),
	LINE,
#ifdef PDCURSES
	elm("~Up", scrollup),
	elm("Do~wn",edscholldown),
#else
	elk("~Up","M-\\", scrollup),
	elk("Do~wn","M-|", edscholldown),
#endif
	LINE,
#ifdef PDCURSES
	elk("~Left item", "M-[",scrollitem),
#else
	elk("~Left item", "M-[,M-[",scrollitem),
#endif
	elk("~Right item","M-]", scrollbackitem),
	LINE,
#ifdef PDCURSES
	elm("Le~ft byte",scrollbyte),
	elm("Ri~ght byte",scrollbackbyte))
#else
	elk("Le~ft byte", "M-{" ,scrollbyte),
	elk("Ri~ght byte", "M-}",scrollbackbyte))
#endif
		
menumake(marks,"Mar~k", 
	elk("~Make","C-g",keymark), 
	elk("~Goto","M-g",tomark), 
	elk( "Ne~xt","Tab",nextmark), 
#ifdef PDCURSES
	elk("~Previous","S-Tab",prevmark), 
#else
	elk("~Previous","M-Tab",prevmark), 
#endif
	LINE,
	elk("~List","M-l",showmarks<0>),
	elm("S~ort",sortmarks),
	elm("S~ubtract",subtractmarks),
	elk("Dele~te","C-d",deletemark),
	elm("~Clear",clearmarks),
	LINE,
	elm("~Save", savemarks),
	elm("~Read", readmarks)) 
menumake(display,"~Data",
	elk("Pre~v","F7",prevmode), 
	elk("~Next","F8",nextmode), 
	LINE,
	elk("~Remove","C-v",removemode), 
	elk("~Full","C-f",zoommode),
	LINE,
	elm("Uns~igned",addunsigned),
	elm("~Ascii",naarASCII),
	elm("Empt~y",emptycontain),
	elm("~Clone",copymode),
	LINE,
EXTRAPROC

	elm("Lis~t", listtypes))

menumake(ondatatype,"T~ype",
	elm("Re~order", reorderunit),
EXTRAFILTERS
	elm("R~m filter", rmlastfilter),
	LINE,
	elm("Re~peat",repeatcontain),
	elm("Confi~g",datatypeconfig),
	elm("Mi~sc", dataconfig),
	LINE,
#ifdef PDCURSES
	elm("Paren~t",uplevel),
	elm("~Child",downlevel),
#else
	elk("Paren~t","M-;",uplevel),
	elk("~Child","M-:",downlevel),
#endif
	elm("S~ub",  newcontain),
	elm("Le~velout",  levelout),
	elm("St~retch",  stretchcontain),
	LINE,
	elm("~Write Type", savedatatype),
	elm("Rea~d Type", readcontain))

menumake(window,"~Window",
	elk("A~dd Prev","S-F5",addprev), 
	elk("~Add Next","S-F6",addnext),
	elk("~Prev","F5",prevfile),  
	elk("~Next","F6",nextfile), 
	LINE,
	elm("~First",firstfile), 
	elm("Las~t",lastfile),
	LINE, 
	elk("~Zoom","M-z",zoomwindow), 
	elk("~Hide","M-h",hidewindow),
	elk("~List","M-0",listfiles))

#if defined(NODL) 
#if defined(FAKE_DYNAMIC)
menumake(options,"~Tools" , 
	elm("~Offset",askbase), 
	elm("~Messageline",switchmessage), 
	elm("Men~u",switchmenu),
	elm("~Commandmode(on/off)",switchcommand),
	LINE,
	elk("~Record keys","C-t",record),
	elm("~Save Recorded",saverecord),
	elm("Pla~y file",playkeysfile),
	elk("Pl~ay","F4",playrecord),
	LINE,
	elm("~Dynamic", selectdynprocedure),
	elm("~Procedures", selectprocedure),
	elm("~Key binding", showprocedure))
#else
menumake(options,"~Tools" , 
	elm("~Offset",askbase), 
	elm("~Messageline",switchmessage), 
	elm("Men~u",switchmenu),
	elm("~Commandmode(on/off)",switchcommand),
	LINE,
	elk("~Record keys","C-t",record),
	elm("~Save Recorded",saverecord),
	elm("Pla~y file",playkeysfile),
	elk("Pl~ay","F4",playrecord),
	LINE,
	elm("~Procedures", selectprocedure),
	elm("~Key binding", showprocedure))
#endif
#else
menumake(options,"~Tools" , 
	elm("~Offset",askbase), 
	elm("~Messageline",switchmessage), 
	elm("Men~u",switchmenu),
	elm("~Commandmode(on/off)",switchcommand),
	LINE,
	elk("~Record","C-t",record),
	elm("~Save Recorded",saverecord),
	elm("Pla~y file",playkeysfile),
	elk("Pl~ay Recorded","F4",playrecord),
	LINE,
	elm("~Procedures", selectprocedure),
	elm("~Dynamic", selectdynprocedure),
	elm("~Load plugin", opendynlib),
	elm("~Key binding", showprocedure))
#endif
