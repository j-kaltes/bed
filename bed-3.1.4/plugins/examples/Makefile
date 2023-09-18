export LC_ALL=C
ifeq ($(HOME),)
	export HOME=/
endif

export SRCPLUGINDIR=..
CONFIGFILE= ../../config

ifeq ($(CONFDIR),)
	export PLUGINDIR=..
	export BEDSRCDIR=../..
else
	export PLUGINDIR=$(CONFDIR)/plugins
endif
include $(CONFIGFILE)
#include  ../rules.mk

DEFINES_H=../defines.h
ifeq ($(CONFIG_STATIC),y)
CFLAGS := $(CFLAGS) -DSTATIC=1
CXXFLAGS := $(CXXFLAGS) -DSTATIC=1
endif
ifeq ($(CONFIG_LARGEFILES),y)
CXXFLAGS := $(CXXFLAGS) -DLARGEFILES=1
endif
ifeq ($(CONFIG_THREADS),y)
CFLAGS := $(CFLAGS) -DUSETHREADS=1  -D_REENTRANT=1
CXXFLAGS := $(CXXFLAGS) -DUSETHREADS=1  -D_REENTRANT=1
endif
ALGFLAGS=-fno-exceptions -fno-rtti
CFLAGS+=$(ALGFLAGS)
CXXFLAGS+=$(ALGFLAGS)
EXAMPLEDIR=$(PLUGINDIR)/examples
EXAMPLEDIRSTR=\"$(EXAMPLEDIR)\"
GENERAL=$(SRCPLUGINDIR)/plugin.cpp
HEADERS=$(SRCPLUGINDIR)/faked.h  $(SRCPLUGINDIR)/mainprocedures.h

LDRELOC=-Ur -d
TOHOSTCXX=$(CXX) 
TOHOSTCC=$(CC) 

#CFLAGS:= -fno-builtin-memcpy -fPIC $(CFLAGS)

BIN2BYTE= ../../utils/bin2byte
PLUGINCFLAGSALL=-DPLUGINSRC=1
PLUGINCXXFLAGSALL=-DPLUGINSRC=1
LDFLAGS+=-fno-exceptions -fno-rtti

ifeq ($(CONFIG_DEBUG),n)
PLUGINLDFLAGS= -O3 -s  $(LDFLAGS)
PLUGINCFLAGS= $(CFLAGS) $(OPTS) -O3 $(PLUGINCFLAGSALL)
CXXFLAGS+= $(OPTS) -O3 $(CXXFLAGSALL)
else
ifeq ($(CONFIG_DEBUG),o)
PLUGINLDFLAGS= -O2 -g $(LDFLAGS)

PLUGINCFLAGS= $(CFLAGS) $(OPTS) -g -O2 $(PLUGINCFLAGSALL) -DDEBUGFLAG
CXXFLAGS+= $(OPTS) -g -O2 $(PLUGINCXXFLAGSALL) -DDEBUGFLAG


else
ifeq ($(CONFIG_DEBUG),m)
PLUGINCFLAGS= $(CFLAGS) $(OPTS) -g $(PLUGINCFLAGSALL) 
CXXFLAGS+= $(OPTS) -g $(PLUGINCXXFLAGSALL)
PLUGINLDFLAGS= -g $(LDFLAGS)
else
PLUGINCFLAGS= $(CFLAGS) $(OPTS) -g $(PLUGINCFLAGSALL) -DDEBUGFLAG
CXXFLAGS+= $(OPTS) -g $(PLUGINCXXFLAGSALL) -DDEBUGFLAG
PLUGINLDFLAGS= -g $(LDFLAGS)

endif
endif
endif
#BEDSRCDIR=\"../../src\"
PLUGINREST := -I../ -I../../src/ -I../../src/empty  $(X_CFLAGS)   -DNEWTERM     -DBEDSRCDIR=$(BEDSRCDIR)
PLUGINCFLAGS := $(PLUGINREST) $(PLUGINCFLAGS) # -m96bit-long-double
CXXFLAGS := $(PLUGINREST) $(CXXFLAGS) # -m96bit-long-double

CFLAGSNOOPT= $(CFLAGS) $(PLUGINCFLAGSALL) $(PLUGINREST)
CXXFLAGSNOOPT= $(CXXFLAGS) $(PLUGINCXXFLAGSALL) $(PLUGINREST)

DYNLINK:= -fPIC 
PLUGINCXXFLAGS:=$(DYNLINK) $(CXXFLAGS) 
LINKINCXXFLAGS:=$(CXXFLAGS) $(PROGRAMC)

#PLUGINCFLAGS=-g
#PLUGINLDFLAGS=-s -O6
#PLUGINLDFLAGS= -O6
#PLUGINLDFLAGS= -g
#PLUGINLDFLAGS+=-Wl,-z,noexecstack
#PLUGINLDFLAGS+=-Wl  -z --noexecstack 

ifneq ($(SYSTEM),SunOS)
#PLUGINLDFLAGS+= -z --noexecstack 
PLUGINLDFLAGS+=-Wl,-z,noexecstack $(LDFLAGS)
endif
LINKINLDFLAGS=$(LDFLAGS) $(PROGRAMLD)


SRC:=   bitcontains.cc bitcpy.cc bitfields.cc bitfields.h bitflags.cc char2.cc chars.c crc.c  duplicate.cpp   edit.cc  file.cpp filter.cpp float.cc disass.cc hash.h disass-use.c nextitem.cpp subtract.cc  signed.cc skip.cpp time.cpp  translate.cc undoall.cpp unsigned.cpp  backward.cc oldsigned.cc  oldsigned.h signed.h  title.cc helpf1.cc searchclip.cc newdata.cc install.cc datatypeinfo.cc cut.cc eventnames.h
TESTSRC:= testall.cpp testbed.cpp testmode.cpp testsave.cpp testsearch.cpp  

DATATYPES:=  char2.plug float.plug subtract.plug signed.plug  unsigned.plug ebcdic2ascii.plug bitcontains.plug bitflags.plug  oldsigned.plug newdata.plug
MISC:= file.plug filter.plug disass-use disass.plug nextitem.plug undoall.plug  skip.plug duplicate.plug edit.plug  translate.plug backward.plug title.plug  helpf1.plug install.plug searchclip.plug datatypeinfo.plug cut.plug multiply.plug

ifeq ($(HAS_X),y)
MISC+= switchsize.plug putselect.plug rxvt.plug
endif
ifeq ($(USES_ELF),y)
DATATYPES+= index2table.plug elftables32.plug
SRC+=index2table.cc elftables32.cc index2table.h
XSRC:= switchsize.cc rxvt.cc
ifeq ($(CONFIG_THREADS),y)
XSRC+= putselect.cc
endif

endif



TESTPLUG=  testall.plug testbed.plug testmode.plug testsave.plug testsearch.plug
PLUGINS= $(DATATYPES)  $(MISC)
GIVEOLD= time.plug  ebcdic2ascii.plug skip.plug file.plug  duplicate.plug nextitem.plug 
GIVENEW= float.plug signed.plug  edit.plug  bitflags.plug bitcontains.plug translate.plug  backward.plug title.plug helpf1.plug searchclip.plug datatypeinfo.plug cut.plug filter.plug  disass.plug  undoall.plug char2.plug
ifeq ($(USES_ELF),y)
GIVEMISC=disass-use 
endif
ifeq ($(ELFSIZE),32)
GIVENEW+=index2table.plug elftables32.plug
endif
ifeq ($(HAS_X),y)
SRC+= $(XSRC)
GIVENEW+= switchsize.plug rxvt.plug
ifeq ($(CONFIG_THREADS),y)
GIVENEW+=putselect.plug 
endif
endif

ifeq ($(CYGWIN),yes)
SRC+=clipboard.cc switchsize.cc rxvt.cc

endif

PREGIVE= $(GIVEOLD) $(GIVENEW)
ifeq ($(CONFIG_PLUGIN),y)
GIVE= $(filter-out $(REMOVEPLUG) $(LINKIN) $(LINKINDYN),$(PREGIVE))
else
GIVE=
endif
#GIVE= $(filter signed.plug,$(PREGIVE))
NOTGIVE= $(filter-out $(PREGIVE),$(PLUGINS))
NONEX= $(filter-out $(PLUGINS),$(GIVE))
INLINK=$(LINKIN:%.plug=%.oo)
INLINKSRC=$(filter $(wildcard *.cc *.cpp *.CPP *.CC), $(wildcard $(LINKIN:%.plug=%.c[cp]*)) )
INLINKDYN=$(LINKINDYN:%.plug=%.ooo)
INLINKDYNSRC=$(filter $(wildcard *.cc *.cpp), $(wildcard $(LINKINDYN:%.plug=%.c[cp]*)) )

#INLINKSRC=$(patsubst %.plug,%.cc %.cpp,$(LINKIN))
EXAMPLES=  $(filter-out $(LINKIN) $(LINKINDYN),$(PLUGINS))
INSTALL=install
GENERATED= ascii2ebcdic.h ebcdic2ascii.h ebcdic2ascii.cc floatdef.h

ALLSRC:=Makefile $(SRC) $(XSRC) $(GENERATED) encodechar.sh macro2plug.sh

ALLFILES :=$(ALLSRC) $(PLUGINS)
GIVEALL:=$(ALLSRC)  $(GIVE)
INLINKALL= $(patsubst bitcontains.%,bitcontains.% bitcpy.o bitfields.o, $(INLINK) $(INLINKDYN) )
ifeq ($(SYSTEM),FreeBSDnot)
all:  nss_dns.so.1 $(EXAMPLES)
else
all:   $(EXAMPLES)
endif
give:  $(GIVE)  $(GIVEMISC)
data:$(DATATYPES)
INBEDRC=$(filter-out switchsize.plug rxvt.plug putselect.plug,$(GIVE))
try:
	@echo examples not GIVES $(NONEX)
	@echo GIVE no examples  $(NOTGIVE)
	@echo EXAMPLES: $(EXAMPLES)
	@echo GIVENEW: $(GIVENEW)	
	@echo give: $(GIVE)	
	@echo linkin: $(LINKIN)
	@echo LINKIN: $(INLINK)
	@echo LINKINDYN: $(INLINKDYN)
	@echo INLINKSRC: $(INLINKSRC)
	@echo INLINKSRC: $(INLINKDYNSRC)
ingive: Makefile
	@printf " $(INBEDRC:%.plug=plugin %.plug\n)" > ingive
	@printf "\nkeys 27 91 49 49 126 = f1\nkeys 27 91 91 65 = f1\n" >> ingive

#	sed --silent -e "{s/^[ 	]*call([ 	]*\([^),; 	.*/\\]*\)[ 	]*)/prodef(\1)/p; s/^[ 	]*callname([ 	]*\([^),; 	.*/\\]*\)[ 	]*)/prodefname(\1)/p; }" $(INLINKSRC) >  $@

../../src/inlink.a: $(INLINKALL) 
	-rm $@
	ar rsc $@ $(INLINKALL)   

../../src/inlink.o: $(INLINKALL) $(CONFIGFILE)
	ld -o $@ $(INLINKALL)   $(LDRELOC)
#grep 'will_be_mentioned_in_procedures_h_.*_var_to_export_it'|
LINKINCCCPP= $(patsubst %.cc,%.ii, $(filter %.cc,$(INLINKSRC)  ))
LINKINCPPCPP= $(patsubst %.cpp,%.ii, $(filter %.cpp,$(INLINKSRC)  ))


ifneq ($(LINKINCCCPP),)
LINKINCPP += $(LINKINCCCPP)
endif
ifneq ($(LINKINCPPCPP),)
LINKINCPP += $(LINKINCPPCPP)
endif


#	grep 'int (Editor::\*will_be_mentioned_in_procedures_h_\([^)]*\)_var_to_export_it)(void)=\&Editor::' $(LINKINCPP)> uitfile

# int (Editor::*for_procedure_h_##proc##_prodefname)(void)=&Editor::proc;char name_part_##proc##_end[]=name;

#	grep -h 'for_procedures_h_\([^)]*\)_prodef' $(LINKINCPP)
tmpmainprocedures.h:
	cp ../../src/procedures.h
inlinker:
	@echo $(LINKINCPPCPP)
	@echo $(LINKINCCCPP)
	@echo .$(LINKINCPP).
inlink.h: $(LINKINCPP)  $(CONFIGFILE) 
ifdef LINKINCPP
	grep -h 'int (Editor::\*for_procedures_h_\([^)]*\)_prodef.*)(void)=\&Editor::\([^;]*\);' $(LINKINCPP) |sed '{s/int (Editor::\*for_procedures_h_\([^)]*\)_prodef)(void)=\&Editor::\([^;]*\);/prodef(\1)/g;s/int (Editor::\*for_procedures_h_\([^)]*\)_prodefname)(void)=\&Editor::\([^;]*\);char name_part_\([^)]*\)_end\[\]=\([^;]*\);/prodefname(\1,\4)/g;}'  > inlink.h
else
	-rm inlink.h
	touch inlink.h
endif


##define callname(name,proc)  int (Editor::*for_procedures_h_##proc##_prodefname)(void)=&Editor::proc;char name_part_##proc##_end[]=name;
#../../src/inlink.h: $(INLINKIN)  $(CONFIGFILE) 
#	sed  '{s/^[ 	]*call([ 	]*\([^),; 	.*/\\]*\)[ 	]*)/prodef(\1)/p; s/^[ 	]*callname([ 	]*\([^),; 	.*/\\]*\)[ 	]*)/prodefname(\1)/p; s/^.*//p; }' $(INLINKSRC) >  $@
#	$(TOHOSTCXX) $(PLUGINCFLAGS) -DPLUGIN -x c   -E -P -D"call(x)=uniekvoorinlinkhprodef(x)" -D"callname(name,x)=uniekvoorinlinkhprodefname(name,x)"   $(INLINKSRC)> tussengegs

bitfields.plug: bitfields.o bitcpy.o
	$(TOHOSTCXX) $(PLUGINLDFLAGS) -shared  $^ -o $@ 
	chmod a-x $@
#s/-DEXAMPLEDIR=$(EXAMPLEDIR)/\-DEXAMPLEDIR\=\$(EXAMPLEDIRSTR)/g

bitfields.o: bitfields.cc
	$(TOHOSTCXX) -DNOEDITOR -DGLOBAL -DUNDER $(PLUGINCXXFLAGS) -DPLUGIN -DEXAMPLEDIR=$(EXAMPLEDIRSTR) -DSOURCEFILENAME=\"examples/$<\" -c $< -o $@

bitcontains.plug: bitcontains.o bitcpy.o bitfields.o
	$(TOHOSTCXX) $(PLUGINLDFLAGS) -shared  $^ -o $@ 
	chmod a-x $@
mkfloatdef: mkfloatdef.cc Makefile
	$(TOBUILDCXX) -DGLOBAL -DUNDER $(CXXFLAGS) -DPLUGIN  -DEXAMPLEDIR=$(EXAMPLEDIRSTR)  -DSOURCEFILENAME=\"examples/$<\"  $< -o $@

floatdef.h: mkfloatdef
	./mkfloatdef > floatdef.h
#float.oo: floatdef.h
#float.ooo: floatdef.h
#float.o: floatdef.h

%.plug: %.o
#	$(TOHOSTCXX) -shared $(PLUGINLDFLAGS) $< func.o -o $@
	$(TOHOSTCXX) -shared $(PLUGINLDFLAGS) $< -o $@
	chmod a-x $@
	rm $<

%.o: %.cpp  $(GENERAL) $(HEADERS)  
	$(TOHOSTCXX) $(PLUGINCXXFLAGS) -DPLUGIN=1 -DEXAMPLEDIR=$(EXAMPLEDIRSTR) -DSOURCEFILENAME=\"examples/$<\" -c $(GENERAL) -o $@

%.o: %.cc $(HEADERS)  
	$(TOHOSTCXX) -DGLOBAL -DUNDER $(PLUGINCXXFLAGS) -DPLUGIN  -DEXAMPLEDIR=$(EXAMPLEDIRSTR)  -DSOURCEFILENAME=\"examples/$<\" -c $< -o $@


elftables32.o: elftables32.cc  $(HEADERS)  
	$(TOHOSTCXX) -g  -DGLOBAL -DUNDER $(PLUGINCXXFLAGS) -DPLUGIN  -DEXAMPLEDIR=$(EXAMPLEDIRSTR)  -DSOURCEFILENAME=\"examples/$<\" -c $< -o $@

ifneq ($(CYGWIN),yes)
XINCL=$(X_CFLAGS)
endif
switchsize.o: switchsize.cc $(HEADERS) 
	$(TOHOSTCXX) $(XINCL) -DGLOBAL -DUNDER $(PLUGINCXXFLAGS) -DPLUGIN  -DEXAMPLEDIR=$(EXAMPLEDIRSTR)  -DSOURCEFILENAME=\"examples/$<\" -c $< -o $@
%.i: %.cc $(HEADERS) 
	$(TOHOSTCXX)  -DGLOBAL -DUNDER -DPLUGIN $(PLUGINCXXFLAGS)  -DEXAMPLEDIR=$(EXAMPLEDIRSTR)  -DSOURCEFILENAME=\"examples/$<\" -E -P $< -o $@

%.oo: %.cc  $(HEADERS)  
	$(TOHOSTCXX)  -DGLOBAL -DUNDER $(LINKINCXXFLAGS)  -DEXAMPLEDIR=$(EXAMPLEDIRSTR)  -DSOURCEFILENAME=\"examples/$<\" -c $< -o $@

#bitcontains.oo: bitcontains.oo bitcpy.oo bitfields.oo
#	ld -o $@ $^   $(LDRELOC)

#bitcontains.ooo: bitcontains.ooo bitcpy.ooo bitfields.ooo
#	ld -o $@ $^ $(LDRELOC)


%.ooo: %.cc  $(HEADERS)  
	$(TOHOSTCXX) -DFAKE_DYNAMIC=1  -DGLOBAL -DUNDER $(LINKINCXXFLAGS)  -DEXAMPLEDIR=$(EXAMPLEDIRSTR) -DSOURCEFILENAME=\"examples/$<\" -c $< -o $@
elftables32.ooo: elftables32.cc  $(HEADERS)  
	$(TOHOSTCXX) -DFAKE_DYNAMIC=1  -DGLOBAL -DUNDER $(CXXFLAGSNOOPT)  -DEXAMPLEDIR=$(EXAMPLEDIRSTR) -DSOURCEFILENAME=\"examples/$<\" -c $< -o $@

%.ooo: %.cpp  $(GENERAL) $(HEADERS) 
	$(TOHOSTCXX) -DFAKE_DYNAMIC $(LINKINCXXFLAGS)  -DEXAMPLEDIR=$(EXAMPLEDIRSTR)  -DSOURCEFILENAME=\"examples/$<\" -c $(GENERAL) -o $@
%.oo: %.cpp  $(GENERAL) $(HEADERS) 
	$(TOHOSTCXX) $(LINKINCXXFLAGS)  -DEXAMPLEDIR=$(EXAMPLEDIRSTR)  -DSOURCEFILENAME=\"examples/$<\" -c $(GENERAL) -o $@

%.iii: %.cc  $(HEADERS)  $(CONFIGFILE)
	$(TOHOSTCXX) -P  -DFAKE_DYNAMIC  -DGLOBAL -DUNDER $(LINKINCXXFLAGS)  -DEXAMPLEDIR=$(EXAMPLEDIRSTR)  -DSOURCEFILENAME=\"examples/$<\" -E $< -o $@
%.iii: %.cpp  $(GENERAL) $(HEADERS) 
	echo hallo
	$(TOHOSTCXX) -P -DFAKE_DYNAMIC $(LINKINCXXFLAGS)  -DEXAMPLEDIR=$(EXAMPLEDIRSTR)  -DSOURCEFILENAME=\"examples/$<\" -E $(GENERAL) -o $@
	$(TOHOSTCXX) -DFAKE_DYNAMIC $(LINKINCXXFLAGS)  -DEXAMPLEDIR=$(EXAMPLEDIRSTR)  -DSOURCEFILENAME=\"examples/$<\" -c $(GENERAL) -o $*.ooo


%.ii: %.cc  $(HEADERS)  $(CONFIGFILE)
	$(TOHOSTCXX)  -DGLOBAL -DUNDER $(LINKINCXXFLAGS)  -DEXAMPLEDIR=$(EXAMPLEDIRSTR)  -DSOURCEFILENAME=\"examples/$<\" -E $< -o $@
#	$(TOHOSTCXX) '-Dwill_be_used_to_get_prodefname_in_inlink_h(x,y)=prodefname(x,y)' '-Uwill_be_used_to_get_prodef_in_inlink_h(x)=prodef(x)' -DGLOBAL -DUNDER $(PLUGINCXXFLAGS)  -DEXAMPLEDIR=$(EXAMPLEDIRSTR)  -DSOURCEFILENAME=\"examples/$<\" -E $< -o $@
#	$(TOHOSTCXX) '-Uwill_be_used_to_get_prodefname_in_inlink_h' '-Uwill_be_used_to_get_prodef_in_inlink_h' -DGLOBAL -DUNDER $(PLUGINCXXFLAGS)  -DEXAMPLEDIR=$(EXAMPLEDIRSTR) -DSOURCEFILENAME=\"examples/$<\" -E $< -o $@

%.ii: %.cpp  $(GENERAL) $(HEADERS) 
	$(TOHOSTCXX)  -P $(LINKINCXXFLAGS)  -DEXAMPLEDIR=$(EXAMPLEDIRSTR)  -DSOURCEFILENAME=\"examples/$<\" -E $(GENERAL) -o $@
%.s: %.cpp  $(GENERAL) $(HEADERS) 
	$(TOHOSTCXX)  $(LINKINCXXFLAGS) -DPLUGIN  -DEXAMPLEDIR=$(EXAMPLEDIRSTR)  -DSOURCEFILENAME=\"examples/$<\" -S $(GENERAL) -o $@

disass-use: disass-use.c 
	$(TOHOSTCC) $(CFLAGSNOOPT) $(LINKINLDFLAGS) -g disass-use.c -o disass-use
#	cd $(EXAMPLEDIR) ;pwd>;cd $(CURRENT)

nss_dns.so.1:./mknss_dns.sh  Makefile ../../config
	sh ./mknss_dns.sh .
install: $(CONFIGFILE)
	-mkdir -p $(ROOTDIR)$(EXAMPLEDIR)
ifeq ($(SYSTEM),FreeBSD)
	sh ./mknss_dns.sh $(ROOTDIR)$(EXAMPLEDIR)
endif
	cp  $(GIVEMISC) $(GIVEALL) $(ROOTDIR)$(EXAMPLEDIR)
	(CURDIR=`pwd`;cd $(ROOTDIR)$(EXAMPLEDIR);chmod 644 $(GIVEALL); cd $$CURDIR)

#	-rm $(EXAMPLEDIR)/disass-use
#	$(MAKE) -C  $(EXAMPLEDIR) disass-use
#	-rm $(EXAMPLEDIR)/disass.plug 
#	$(MAKE) -C  $(EXAMPLEDIR) disass.plug

%.i: %.cpp  $(GENERAL) $(HEADERS) 
	$(TOHOSTCXX) -DPLUGIN $(PLUGINCXXFLAGS)  -DEXAMPLEDIR=$(EXAMPLEDIRSTR)  -DSOURCEFILENAME=\"examples/$<\" -E -P $(GENERAL) -o $@

clean:
	-rm *.plug *.o *.oo *.ooo  disass-use$(HOSTEXEEXT)  core ingive *.ii *.i *.iii

realclean: clean
	-rm   mkhelp$(BUILDEXEEXT) mkman$(BUILDEXEEXT)
	-rm plugman plughelp inlinklist inlink.h mkhelp.h mkman.h
ifneq ($(CYGWIN),yes)
	-rm chars
else
	-rm chars.exe
endif

switchsize.plug: switchsize.o
	$(TOHOSTCXX) $(PLUGINLDFLAGS) -shared $< -o $@ $(X_LIBS) -lX11 $(X_EXTRA_LIBS)
	chmod a-x $@
putselect.plug: putselect.o
	$(TOHOSTCXX) $(PLUGINLDFLAGS) -shared $< -o $@  $(X_LIBS) -lX11 $(X_EXTRA_LIBS)
	chmod a-x $@
switchsize: switchsize.cc
	echo $(CYGWIN)
ifneq ($(CYGWIN),yes)
	$(TOHOSTCXX)  -I../../src -g -DTEST  $< -o $@ $(X_LIBS) -lX11 $(X_EXTRA_LIBS)
else
	$(TOHOSTCXX) -I../../src -D_WIN32 -g -DTEST  $< -o $@ 
endif

disass.plug: disass.o disass-use
	$(TOHOSTCXX)  $(PLUGINLDFLAGS)  -shared $< -o $@
	chmod a-x $@
time: time.cpp
	$(TOHOSTCXX) -g -DTEST -DGLOBAL $< -o $@  $(X_LIBS)  -lX11  $(X_EXTRA_LIBS)


chars.bin: chars.c
	$(TOBUILDCC)  chars.c -o chars
	./chars > chars.bin
ifneq ($(CYGWIN),yes)
	rm chars
else
	rm chars.exe
endif

ebcdic2ascii.cc: ascii2ebcdic.h ebcdic2ascii.h ./encodechar.sh 
	sh ./encodechar.sh ebcdic ascii
ebcdic2ascii.h:
	dd if=chars.bin  conv=ascii of=ebcdic2a.sci bs=256 count=1 
ifeq ($(SYSTEM),FreeBSD)
	@if cmp -s chars.bin ebcdic2a.sci; then echo ERROR!!! No conversion applied!!! ; fi
endif
	$(BIN2BYTE) ebcdic2a.sci ebcdic2ascii ebcdic2ascii.h 
#	rm ebcdic2a.sci
ascii2ebcdic.h:
	dd if=chars.bin  conv=ebcdic of=ascii2eb.cdi bs=256 count=1 
	@if cmp -s chars.bin ascii2eb.cdi; then echo ERROR!!! No conversion applied!!! ; fi
	$(BIN2BYTE) ascii2eb.cdi ascii2ebcdic  ascii2ebcdic.h 
#	rm  ascii2eb.cdi


MACROFUNCTION=MacroFuctionNameShouldBeLongerThenEveryUsedFunctioName
MACRODATA=12345678BeginMacroKeyStrokes
MACROFLAGS=-DMACROFUNCTION=\"$(MACROFUNCTION)\" -DMACRODATA=\"$(MACRODATA)\"
#macrotemplate.plug: macrotemplate.cpp Makefile ./fsize $(CONFIGFILE)
#	$(TOHOSTCXX) $(PLUGINLDFLAGS) -DPLUGIN -shared $(MACROFLAGS) $(PLUGINCFLAGS)  -DEXAMPLEDIR=$(EXAMPLEDIRSTR)  -DSOURCEFILENAME=\"examples/$<\"  $(GENERAL) -o $@

#macrotemplate.o: macrotemplate.cc Makefile ./fsize $(CONFIGFILE)
#	$(TOHOSTCXX)  -DGLOBAL -DUNDER $(MACROFLAGS) $(PLUGINCFLAGS) -DPLUGIN  -DEXAMPLEDIR=$(EXAMPLEDIRSTR)  -DSOURCEFILENAME=\"examples/$<\" -c $< -o $@ 
#macrotemplate.plug: macrotemplate.o
#	ld -shared  $< -o $@
macrotemplate.plug: macrotemplate.cc Makefile ./fsize $(CONFIGFILE)
	$(TOHOSTCXX) -shared -DGLOBAL -DUNDER $(MACROFLAGS) $(PLUGINCXXFLAGS) -DPLUGIN  -DEXAMPLEDIR=$(EXAMPLEDIRSTR)  -DSOURCEFILENAME=\"examples/$<\"  $< -o $@
 
#	$(TOHOSTCXX) $(PLUGINLDFLAGS) -DPLUGIN -shared $(MACROFLAGS) $(PLUGINCFLAGS)  -DEXAMPLEDIR=$(EXAMPLEDIRSTR)  -DSOURCEFILENAME=\"examples/$<\" $< -o $@


macrotemplate.i: macrotemplate.cpp Makefile 
	$(TOHOSTCXX) $(PLUGINLDFLAGS)  -DPLUGIN -E -shared $(MACROFLAGS) $(PLUGINCXXFLAGS)  -DEXAMPLEDIR=$(EXAMPLEDIRSTR)  -DSOURCEFILENAME=\"examples/$<\"  $(GENERAL) -o $@

places.h: macrotemplate.plug pos
	echo -n "int macrofunctionpos=" > places.h
	./pos macrotemplate.plug $(MACROFUNCTION) >>places.h
	echo ";" >>places.h
	echo -n "int macrodatapos=" >> places.h
	./pos macrotemplate.plug $(MACRODATA) >>places.h
	echo ";" >>places.h


macroplug.o: macroplug.cpp  $(GENERAL) $(HEADERS)  places.h $(CONFIGFILE)
	$(TOHOSTCXX) $(PLUGINCXXFLAGS) -DPLUGIN  -DEXAMPLEDIR=$(EXAMPLEDIRSTR)  -DSOURCEFILENAME=\"examples/$<\" -c $(GENERAL) -o $@
macroplug.oo: macroplug.cpp  $(GENERAL) $(HEADERS)  places.h $(CONFIGFILE)
	$(TOHOSTCXX) $(LINKINCXXFLAGS)  -DEXAMPLEDIR=$(EXAMPLEDIRSTR)  -DSOURCEFILENAME=\"examples/$<\" -c $(GENERAL) -o $@

../mainprocedures.h:
	$(MAKE) -C .. mainprocedures.h

plughelp: mkhelp 
	./mkhelp > plughelp
mkhelp: mkhelp.h mkhelp.c
	$(TOBUILDCC)  -DHELP_H='"mkhelp.h"' mkhelp.c -o mkhelp
mkhelp.h: $(INLINKSRC)  mkhelp.sh $(CONFIG)
	sh ./mkhelp.sh  $(INLINKSRC)  > mkhelp.h

plugman: mkman 
	./mkman > plugman
mkman: mkman.h mkhelp.c
	$(TOBUILDCC)  -DHELP_H='"mkman.h"' mkhelp.c -o mkman
mkman.h: $(SRC) mkhelp.sh 
	sh ./mkhelp.sh $(filter-out $(INLINKSRC),$(SRC)) > mkman.h

#|sed -e 's/helpteksten(\([^ 	,]*\),[^"]*"\([^"]*\)".*/\1 \2/g'|sed -e 's/\\n/newline\/g' > mkhelp.h
head:	
	echo $(SRCPLUGINDIR)
	echo $(CFLAGS)
	echo $(CXXFLAGS)
	echo $(LDFLAGS)
	echo $(HEADERS)
	echo $(PLUGINCFLAGS) 
	echo $(PLUGINCXXFLAGS) 

.PHONY: compileinlink makeinlinklist mkinlink_h
compileinlink: $(INLINKALL)
mkinlink_h:   inlink.h
inlinklist: $(CONFIG) inlink.h
	echo $(INLINKALL) > inlinklist
makeinlinklist:
	echo $(INLINKALL) > inlinklist
dep: mkinlink_h makeinlinklist
cds:
	(CURDIR=`pwd`;cd $(ROOTDIR)$(CONFDIR);pwd; cd $$CURDIR;pwd)

showinlink:
	@echo $(INLINKALL) 

srcnum: $(SRC)
	sh mksrcnum.sh srcnum

showsrc:
	@echo $(SRC)


vreemd:
	(CURDIR=`pwd`;echo  $(ROOTDIR)$(EXAMPLEDIR);echo  $(ROOTDIR)$(GIVEALL); echo $$CURDIR)


