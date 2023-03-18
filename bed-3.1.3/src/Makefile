include ../config
include rules.mk
ifeq ($(PDCURSES)$(CYGWIN),1yes2)
WINPD=y
endif
LIBS+= $(CURSESLIB) $(EXTRALIBS) $(OTHERLIBS)# -L/mnt/chicken/lib -L/mnt/chicken/usr/lib
CFLAGS+=  -D_GNU_SOURCE -DNEWTERM #-DUSE_NEW # -DMEMORY_TRACE	#-DREAD0x2x4DATATYPE #-DSEARCHTIME #-DUSE_NEW #-DMEMORY_TRACE	
CXXFLAGS+=  -D_GNU_SOURCE -DNEWTERM #-DUSE_NEW # -DMEMORY_TRACE	#-DREAD0x2x4DATATYPE #-DSEARCHTIME #-DUSE_NEW #-DMEMORY_TRACE	

OPTS:= $(OPTS) 
LIBPATH=-L. 
INLINKLISTER= ../plugins/examples/inlinklist
INLINKLIST= compileinlink
MKINLINK_H=mkinlink_h
ININLINKLIB= $(patsubst %,../plugins/examples/%,$(shell cat $(INLINKLISTER) ))
#CLEANOBJECTS=$(OBJECTS)
OBJECTS+=$(ININLINKLIB)
PROGCC=$(GXX)
ifeq ($(CYGWIN),yes)
SORT=/usr/bin/sort 
else
SORT:=sort 
endif
CP=cp -f -b
SHELL:=bash
BIN2BYTE:= ../utils/bin2byte
MV=mv

#CFLAGS:= -Wall $(CFLAGS) -DSETSCRREGBUG # -DALWAYSEXPLORESIZE 
#CFLAGS:=  -Wno-deprecated  $(CFLAGS) -DSETSCRREGBUG # -DALWAYSEXPLORESIZE 
#CFLAGS:= -Wunused  -Wunused-value  -funsigned-char  $(CFLAGS) -DSETSCRREGBUG # -DALWAYSEXPLORESIZE 
#CFLAGS:= -Wall    -funsigned-char  $(CFLAGS) -DSETSCRREGBUG # -DALWAYSEXPLORESIZE 
CFLAGS:= -Wall     $(CFLAGS) -DSETSCRREGBUG # -DALWAYSEXPLORESIZE 
CXXFLAGS:= -Wall     $(CXXFLAGS) -DSETSCRREGBUG # -DALWAYSEXPLORESIZE 
ifeq ($(SYSTEM),Linux)
#CFLAGS+= -DPROCLOCKSBUG  -DATOIBUG
#CFLAGS+= -DPROCLOCKSBUG  -DATOIBUG
endif

#-DENDOFLINEBUG
OLDCFLAGS:=$(CFLAGS)
CFLAGS +=$(OPTS) $(PROGRAMC)

PCXXFLAGS:=$(CXXFLAGS) $(PROGRAMC)
CXXFLAGS +=$(OPTS) $(PROGRAMC)


LDFLAGS:=$(OPTS) $(LDFLAGS) $(PROGRAMLD)
WCXXFLAGS=$(CXXFLAGS)
# *** NORMALLY NOTHING HAS TO BE CHANGED BELOW THIS LINE

OBJ := repeat.o rbtree.o memory.o editor.o   dialog.o glob.o expandfilename.o readconfig.o  getfree.o getlabel.o  screenpart.o  marks.o digitstring.o   parts.o main.o    keyproc.o marken.o procview.o menus.o inout.o move.o window.o edit.o file.o suspend.o select.o undo.o screen.o search.o onlykey.o  editup.o  cursesinit.o input.o contain.o withcontain.o filter.o  ascii.o  multidigit.o macros.o editmacros.o typelist.o $(OBJ) keyinput.o  hassub.o savedatatype.o menufuncs.o support.o desturbe.o gethelp.o disk.o   memed.o  gnusearch.o unitsearch.o bytesearch.o  gnuasciiregex.o showlist.o  times.o sparsecp.o winsparsecp.o

ifeq ($(USE_HYPERSCAN),1)
OBJ +=hyperscan.o 
endif

ifeq ($(USE_RE2),1)
OBJ += re2search.o  
endif
BUILDOBJ := hostreadhelp.o hostgetlabel.o

ifeq ($(SYSTEM),FreeBSD)
OBJ+=prevention.o
endif

ifneq ($(WINPD),y)
OBJ+=keyconfig.o 
endif
CPPFILES=  debug.cpp editlimits.cpp editor.cpp dialog.cpp getfree.cpp getlabel.cpp digitstring.cpp main.cpp marks.cpp memory.cpp  parts.cpp rbtree.cpp readconfig.cpp screenpart.cpp  trace.cpp keyproc.cpp marken.cpp procview.cpp menus.cpp inout.cpp move.cpp window.cpp edit.cpp file.cpp suspend.cpp select.cpp undo.cpp screen.cpp search.cpp onlykey.cpp editup.cpp myalloc.cpp cursesinit.cpp input.cpp contain.cpp withcontain.cpp filter.cpp  ascii.cpp  multidigit.cpp macros.cpp editmacros.cpp typelist.cc keyconfig.cpp keyinput.cpp repeat.cpp hassub.cpp savedatatype.cpp menufuncs.cpp  support.cpp bedrcproc.cpp desturbe.cpp gethelp.cpp disk.cpp memed.cpp re2search.cpp hyperscan.cpp gnusearch.cpp unitsearch.cpp bytesearch.cpp gnuasciiregex.cpp times.cpp sparsecp.cpp winsparsecp.cpp





CFILES=expandfilename.cpp glob.c 

ifeq ($(SYSTEM),FreeBSD)
CFILES+=prevention.c 
endif
PRESRC=$(CFILES) $(CPPFILES)
SRC=$(PRESRC) version.cpp
GENHEADERS= procedures.h copy.h help.h bedrcproc.h

#HANDHEADERS= builtinkeys.h  config.h convert.h cursesinit.h datatypes.h debug.h editor.h dialog.h getlabel.h glob.h head.h digitstring.h inout.h keyinput.h keylist.h main.h marks.h memory.h myalloc.h offtypes.h onlykey.h  rbtree.h readconfig.h screen.h screenpart.h system.h trace.h type.h views.h trace.cpp menu.h multidigit.h ascii.h  contain.h  filter.h  reverse.h macros.h keystrokes.h typelist.h visuals.h keyproc.h repeat.h hassub.h varmacro.h execproc.h desturbe.h shell.h termkeys.h viewbuf.h


HANDHEADERS= ascii.h biosdisk.h builtinkeys.h confdefines.h conf-.h config.h contain.h convert.h cursesinit.h dataprocer.h dataproc.h datatypes.h debug.h defines.h definesonly.h desturbe.h dialog.h digitstring.h editor.h execproc.h extrakeys.h filter.h getlabel.h glob.h hash.h hassub.h head.h helpdata.h help.h helpindex.h helptext.h inout.h keyinput.h keylist.h keyproc.h keystrokes.h macros.h main.h mainprocedures.h marks.h maxmenus.h memory.h menu.h misc.h mkmenus.h multidigit.h myalloc.h nepsem.h offtypes.h onlykey.h partypes.h  procedures.h ptypes.h rbtree.h readconfig.h regclass.h repeat.h reverse.h screen.h screenpart.h searchaim.h searchinterrupt.h selectsearch.h setenv.h shell.h system.h sys_types.h termkeys.h trace.h type.h typelist.h unsorted.h varmacro.h viewbuf.h views.h visuals.h win.h winpad.h



HEADERS= $(HANDHEADERS) $(GENHEADERS)
OTHERGEN= help
OTHERHAND= preprocedures head.h copy
OTHERFILES= $(OTHERHAND)  $(OTHERGEN)
OLDFILES= oldundo.cpp
SCRIPTS=mkdate.sh mkmachine.sh rules.mk
UPDATES=sourcenumber date.c md5sums.cpp
GENERATED= $(GENHEADERS) $(OTHERGEN)  md5file  md5file  mainprocedures.h extrakeys.h helpdata helpdata.h helpindex.h helptext.h chhelp unsorted.h 
PROGRAMS=extramenunr rxvtmenu  readhelp addmenu

ifeq ($(BUILDEXEEXT),)
EXEPROGRAMS= $(PROGRAMS)
else
ifeq ($(CYGWIN),)
EXEPROGRAMS= $(patsubst %,%$(BUILDEXEEXT),$(PROGRAMS)) 
else
EXEPROGRAMS= $(patsubst %,%$(BUILDEXEEXT),$(PROGRAMS)) winpaden$(BUILDEXEXT)
endif
endif

ifneq ($(origin USE_NEW),undefined)
OBJ := $(OBJ) myalloc.o
endif


ALLEXCEPTVERSION=$(SRC) $(HANDHEADERS) $(OTHERHAND) $(SCRIPTS)

ALLSOURCES=$(ALLEXCEPTVERSION) $(UPDATED)
ALLES= $(ALLSOURCES) $(CONFFILE) $(GENERATED)

all: mkinlinklist plugnum procedures.h $(PROGRAM)
	@echo done

.PHONY: plugnum 

plugnum:
	make -C ../plugins/examples/ srcnum
bash:
	bash
tags: $(CFILES) $(CPPFILES) Makefile
	ctags    $(CFILES) $(CPPFILES) $(HEADERS)
#	ctags  --sort  --c-types=cdefgmnstuvxAC $(CFILES) $(CPPFILES)



.PHONY: back
back: bak $(ALLES)
	cp $(ALLES) bak
bak:
	mkdir bak

$(INLINKLISTER):
	$(MAKE) -C ../plugins/examples makeinlinklist

#PROGCC=$(CXX)

.PHONY: src


src:
	@echo $(ALLES)

ifeq ($(CYGWIN),yes)
dep: winpad.h $(PRESRC) ../config Makefile extrakeys.h dataprocer.h dataproc.h procedures.h mainprocedures.h readhelp.cpp  
else
dep:  $(PRESRC) ../config Makefile extrakeys.h dataprocer.h dataproc.h procedures.h mainprocedures.h readhelp.cpp  
endif
	$(MAKE) -C ../plugins/examples makeinlinklist
	$(PROGCC) -MM -MG $(WCXXFLAGS) $(CFILE) $(CPPFILES) readhelp.cpp > depend

winpad.h: 
	./winpaden > winpad.h
empty:
	@echo done
KEYCONFIG= ../utils/keyconfig

keyconfig:$(KEYCONFIG)

$(KEYCONFIG): bed	
	-rm $(KEYCONFIG)
	ln $(PROGRAM) $(KEYCONFIG)

#PROGCC=gcc

showconfig:
ifeq ($(BIOSDISK),y)
	@echo  "BIOSDISK " $(CYGWINDLL)
endif
ifeq ($(CONFIG_THREADS),y)
	@echo  "Background search "
endif
ifeq ($(CONFIG_PLUGIN),y)
	@echo  "Plugins "
else
	@echo  "No Plugins "
endif

$(PROGRAM):   $(OBJ)  version.o  $(OBJECTS)  
	$(PROGCC)   $(LDFLAGS) $(RDYNAMIC) -o $(PROGRAM)  version.o $(OBJ) $(OBJECTS) $(LIBPATH) $(LIBS)
ifeq ($(SYSTEM),Android)
#	 perl -pi -e 's/libc\+\+_shared.so/libc++.so\000\000\000\000\000\000\000/g'  bed
endif

quick:  $(OBJ)  sourcenumber
	$(PROGCC) $(LDFLAGS) -o $(PROGRAM)  version.o $(OBJ)  $(OBJECTS) $(LIBPATH) $(LIBS)
#sourcenumber: $(SRC)  $(BIN2BYTE)
#	bash ./mkversion $(ALLEXCEPTVERSION)
#md5file: $(SRC)  $(BIN2BYTE)
#	bash ./mkversion $(ALLEXCEPTVERSION)

link:  dataprocer.h menufuncs.o procview.o menus.o  $(INLINKLIST) $(ININLINKLIB) 
	$(PROGCC) $(LDFLAGS) -o $(PROGRAM)  version.o $(OBJ)  $(OBJECTS) $(LIBPATH) $(LIBS)
linkonly:   
	$(PROGCC)  $(LDFLAGS) -o $(PROGRAM)  version.o $(OBJ)  $(OBJECTS) $(LIBPATH)  $(LIBS)

lines: 
	-rm screenpart.o editup.o editor.o
	$(MAKE) bed
$(BIN2BYTE):
	$(MAKE) -C ../utils bin2byte
unsorted.h: preprocedures  dataproc.h ../config
	 $(PROGCC) -DNOINLINK $(PCXXFLAGS) -x c++   -E -P  $<  -o unsorted.h

mainprocedures.h: unsorted.h
	$(SORT) -b -t '(' -k 2 unsorted.h -o  $@

showproc: preprocedures  ../plugins/examples/inlink.h dataproc.h ../config
	echo 	 $(PROGCC) $(PCXXFLAGS) -x c++   -E -P  $<  
procedures.h: preprocedures  ../plugins/examples/inlink.h dataproc.h ../config
	echo 	 $(PROGCC) $(PCXXFLAGS) -x c++   -E -P  $<  
	 $(PROGCC) $(PCXXFLAGS) -x c++   -E -P  $<  | $(SORT) -b -t '(' -k 2 > newprocedures.h
	(diff -q procedures.h newprocedures.h	&& echo no change &&rm newprocedures.h)||mv newprocedures.h procedures.h
	
help: usagehelp.in helpoldterm.c Makefile ../config cygwinhelp helptext
	cat usagehelp.in >  help
#	$(PROGCC)   $(CFLAGS) -x c++ -E -P usagehelp.in|sed -e s/\"//g > help
	echo '-f bedrc: Use configuration file bedrc, instead of env $(CONFVAR)' >>help
ifneq ($(CYGWIN),yes)
	echo '   or $(PERSCONFFILE) or $(GLOBALCONFFILE)' >>help
else
	echo -n '   or %APPDATA%\$(CONFFILE) or ' >>help
	cygpath -w $(GLOBALCONFFILE) >> help
endif
#	echo '-w: Always test filesize' >>help
ifeq ($(CONFIG_PLUGIN),y)
	echo	'-p plugin: load plugin at startup' >> help
endif
ifneq ($(WINPD),y)
	echo '-k file: Configure key bindings for current terminal. Append to file.' >> help
endif
ifeq ($(CONFIG_COLOR),y)
	echo '-B: black-white' >>help
	echo '-C: color' >>help
else
ifeq ($(CONFIG_COLOR),o)
	echo '-C: color' >>help
endif
endif
ifneq ($(filter -DNEWTERM,$(CFLAGS)),-DNEWTERM)
	$(PROGCC)   -imacros config.h $(CFLAGS) -E -P helpoldterm.c >> help
endif
ifeq ($(CYGWIN),yes)
	cat cygwinhelp >> help
endif
	cat helptext >> help

help.h:help $(BIN2BYTE)
	$(BIN2BYTE) help help help.h
	echo >> help.h
copy.h:copy $(BIN2BYTE)
	$(BIN2BYTE) copy copy copy.h
	echo >> copy.h
compileinfo:../config Makefile rules.mk
	echo -e  CXXFLAGS=$(CXXFLAGS)\\nCFLAGS=$(CFLAGS)\\nLDFLAGS=$(LDFLAGS)\\nLIBS=$(LIBS)\\n > compileinfo
definesonly.h: confdefines.h $(HOSTCONF)
	cat $^|grep '^#define'  > definesonly.h
md5sums.cpp:  machine   compileinfo  definesonly.h $(BIN2BYTE)
#	cat machine compileinfo md5file> compinfo
	cat machine compileinfo definesonly.h > compinfo
	 $(BIN2BYTE) compinfo md5sums md5sums.cpp
	echo >> md5sums.cpp

#	 $(BIN2BYTE) md5file md5sums md5sums.cpp

#version.o:  version.cpp sourcenumber md5sums.cpp help.h copy.h

#	$(PROGCC) $(CFLAGS) -c version.cpp



#menus.o: menus.cpp
#	g++  -O2   $(OLDCFLAGS)  -c -o menus.o menus.cpp

debug:
	-rm main.o trace.o debug.o editor.o myalloc.o
	$(MAKE)
mmap:
	-rm memory.o
	$(MAKE)
clean: 
	-rm    $(OBJ) version.o core  debug.o trace.o myalloc.o inlink.a  depend.bak $(CLEANOBJECTS)   $(BUILDOBJ)
	-rm $(EXEPROGRAMS) $(PROGRAM)$(HOSTEXEEXT)
	
#	-rm rxvtmenu $(PROGRAM) core  inlink.a *.o*
realclean: clean
	-rm $(REGEX_DIR)/regex.o
	-rm $(GENERATED) 
	-rm md5sums.cpp #tags 
	-rm depend

machine: sourcenumber
	bash ./mkmachine.sh

date.c:  sourcenumber
	bash ./mkdate.sh
version.o: version.cpp sourcenumber md5sums.cpp copy.h help.h  date.c  ../config config.h
	$(PROGCC) $(CFLAGS) -c $<

digitstring: digitstring.cpp
	$(CXX) -g digitstring.cpp -DTEST -o digitstring 
getfree: getfree.cpp
	$(CXX) $(CXXFLAGS) getfree.cpp -DTEST -o getfree 


#%.o:%.i

#	$(PROGCC)  $(CXXFLAGS) $< -o $@
%.i:%.cpp
	$(PROGCC) -E -P $(CXXFLAGS) $< -o $@
#	$(GCC) -E  $(CPPFLAGS) $< -o $@
%.s:%.cpp
	$(PROGCC) -S  $(CXXFLAGS) $< -o $@
%.i:%.c
	$(PROGCC) -E -P $(CFLAGS) $< -o $@


stupid: inlink.h
	touch stupid
.PHONY: mkinlinklist plugs  mkinlink_h

mkinlink_h:
	$(MAKE) -C ../plugins/examples/ mkinlink_h
mkinlinklist:
	$(MAKE) -C ../plugins/examples/ $(INLINKLIST)
#	$(MAKE) -C ../plugins/examples/ $(@F)
	
#plugs:
#	for plug in $(ININLINKLIB); do \
#	$(MAKE) -C ../plugins/examples/ $(@F); \
#	done

plugs:	$(ININLINKLIB)
../plugins/examples/%: 
	$(MAKE) -C ../plugins/examples/ $(@F)
../plugins/examples/inlink.h: 
	$(MAKE) -C ../plugins/examples/ inlink.h

$(ININLINKLIB):
	$(MAKE) -C ../plugins/examples/ $(@F)

grep.%:
	grep $* *.c* *.h

include dataproc.mk


getch:getch.c 
	$(GCC) -g $(CFLAGS) -UHAS_BOOL getch.c -o getch $(LIBS)
getcher:getcher.c 
	$(GCC) -g $(CFLAGS) -UHAS_BOOL getcher.c -o getcher $(LIBS)


#memory.o:memory.i
#	$(GCC) -c $(CFLAGS) -x c++ memory.i


support: support.cpp expandfilename.cpp
	$(CC) -g -DTEST $^ -o support

keyproc.o: builtinkeys.h
keyproc.o: extrakeys.h
readconfig.o:bedrcproc.h
readconfig:readconfig.o
	$(PROGCC) $(LDFLAGS) -o readconfig readconfig.o -lpdcurses
bedrcproc.h: bedrcproc.cpp
	echo '/*WARNING automatically generated  from bedrcproc.cpp */' > bedrcproc.h
	grep '^initword' bedrcproc.cpp|sed -e 's/^initword\([^{]*\).*/initword\1/'|$(SORT) -b -t '(' -k 2 >> bedrcproc.h

extramenunr$(BUILDEXEEXT):menus.cpp editor.h menu.h
	$(TOBUILDCXX) -DPRINTMAXSHOWMENUS=1 $(TOBUILDCXXFLAGS) extramenunr.cpp -o extramenunr
extramenunr.i:menus.cpp editor.h menu.h
	$(TOBUILDCXX) -E -P -DPRINTMAXSHOWMENUS=1 $(TOBUILDCXXFLAGS) extramenunr.cpp -o extramenunr.i
mkmenus.h: extramenunr$(BUILDEXEEXT)
#	extramenus=`./extramenunr`; for ((i=0;$$i<$$extramenus;i=$$[$$i+1])); do echo  "mkmenunum($$i)";done >mkmenus.h
	extramenus=`./extramenunr`;i=0; while test $$i -lt $$extramenus; do echo  "mkmenunum($$i)";i=$$[$$i+1];done >mkmenus.h
#	havemenus=`gcc -E -P '-Dmenumake(x...)=xoneitem' menu.h|grep xoneitem|wc -w`; for ((i=0;$$i<$$[30-$$havemenus];i=$$[$$i+1])); do echo  "mkmenunum($$i)";done >mkmenus.h
-include depend	

exes:
	echo $(EXEPROGRAMS)

Bed:   
	$(PROGCC) $(LDFLAGS) -o Bed  version.o $(OBJ)  $(OBJECTS)  $(ININLINKLIB) $(LIBPATH) /home/jka/self/projects/c/bedall/libs/libc.so.6 /home/jka/self/projects/c/bedall/libs/libpthread.so.0 /home/jka/self/projects/c/bedall/libs/libdl.so.2  /home/jka/self/projects/c/bedall/libs/libm.so.6  /home/jka/self/projects/c/bedall/libs/libncurses.so.4   /home/jka/self/projects/c/bedall/libs/ld-linux.so.2


beds:   
	$(PROGCC) $(LDFLAGS) -o $(PROGRAM)s  version.o $(OBJ) $(OBJECTS) $(LIBPATH) $(LIBS) -L/usr/X11R6/lib -lX11

hostreadhelp.o: readhelp.cpp procedures.h 
	$(TOBUILDCXX) -c $(TOBUILDCXXFLAGS) readhelp.cpp  -o hostreadhelp.o
hostgetlabel.o: getlabel.cpp 
	$(TOBUILDCXX) -c $(TOBUILDCXXFLAGS) getlabel.cpp  -o hostgetlabel.o

readhelp: hostreadhelp.o hostgetlabel.o 
	$(TOBUILDCXX)  $(TOBUILDLDFLAGS) hostreadhelp.o hostgetlabel.o -o readhelp
helpdata.h: helpdata readhelp
	./readhelp < helpdata
helptext.h: helpdata.h
#	./readhelp < helpdata
helpindex.h: helpdata.h
#	./readhelp < helpdata

ifeq ($(CYGWIN),yes)
helpdata: helpdata.in helpdiskwindows chhelp #../plugins/examples/plughelp
	cat helpdata.in helpdiskwindows ../plugins/examples/plughelp|sed -f chhelp  > helpdata
else
helpdata: helpdata.in helpdisklinux chhelp #../plugins/examples/plughelp
	cat helpdata.in helpdisklinux ../plugins/examples/plughelp|sed -f chhelp  > helpdata
endif


.PHONY: ../plugins/examples/plughelp
../plugins/examples/plughelp:
	$(MAKE) -C ../plugins/examples/ plughelp
chhelp: addmenu
	./addmenu > chhelp
	echo 's/proconly \(\<[^         ]*\>\)/\1/g' >> chhelp
addmenu: addmenu.cpp menu.h
	$(TOBUILDCXX) addmenu.cpp -o addmenu

showopts:
	echo OPTS: $(OPTS)
	echo CFLAGS: $(CFLAGS)
	echo CXXFLAGS: $(CXXFLAGS)
	echo LDFLAGS: $(LDFLAGS)

inin:../plugins/examples/inlinklist showargs
	echo $(ININLINKLIB)
.PHONY: showargs
showargs:
	@echo obj: $(OBJ)
	@echo objects: $(OBJECTS) 
	@echo libpath $(LIBPATH)
	@echo libs: $(LIBS)




#sourcenumber: $(SRC)  
#	bash ./mkversion $(ALLEXCEPTVERSION)
sourcenumber: $(ALLEXCEPTVERSION)  Makefile ../plugins/examples/srcnum
	bash ../plugins/examples/mksrcnum.sh sourcenumber
#	bash ./mkversion
made:
	@echo $(ALLEXCEPTVERSION)

partypes.h: mkpartypes
	./mkpartypes > partypes.h




other:
	echo $(OTHERLIBS)

libtest.a: $(OBJ) $(OBJECTS) version.o 
	ar rc libtest.a $^
rbtest: rbtree.cpp
	$(CXX) -DXViewer -DTEST $(CXXFLAGS) $(LDFLAGS) $^ -o $@ 
testmem:memory.o rbtree.o $(REGEX_DIR)/regex.o  libtest.a
	$(CXX) $(LDFLAGS) $^ -o $@ -L. $(LIBS) -ltest
