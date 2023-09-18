include ../config
include ../prognames.mk
ifeq ($(FULLUNIX)$(HAS_NROFF),ny)
all: datadir.txt bed.1 bed.html bed.txt
else

ifeq ($(SYSTEM),Linux)
all: datadir.txt bed.1.gz
else
all: datadir.txt bed.1
endif
endif
ifneq ($(CYGWIN),yes)
GLOBALCONFIG=$(shell echo '$(GLOBALCONFFILE)'| sed -e 's/\//\\\//g' )
PERSCONFIG=$(shell echo '$(PERSCONFFILE)'| sed -e 's/\//\\\//g' )
CONFDIRLINKER=$(shell echo '$(CONFDIRLINK)'| sed -e 's/\//\\\//g' )
else
#PERSCONFIG=\%LOCALAPPDATA\%\/$(CONFFILE)
PERSCONFIG=\%LOCALAPPDATA\%\\\\$(CONFFILE)
CONFDIRLINKER=$(shell echo `cygpath --windows $(CONFDIRLINK)` | sed -e 's/\\/\\\\\\\\/g' )
GLOBALCONFIG=$(shell echo `cygpath --windows $(GLOBALCONFFILE)` | sed -e 's/\\/\\\\\\\\/g' )
endif


vars:
	echo $(PERSCONFIG)
	echo $(CONFDIRLINKER)
	echo $(CONFDIRLINK)
	
bed.txt: bed.html Makefile
	(elinks   -no-references  -no-numbering -dump bed.html||lynx  -nolist -dump bed.html||nroff -c -mandoc bed.1 ) > bed.txt
ifeq ($(CYGWIN),yes)
	u2d bed.txt
endif 

datadir.txt: .datadir.txt ../.common.txt
	cat .datadir.txt > datadir.txt
	sed 's/CONFDIRLINK/$(CONFDIRLINKER)/g' < ../.common.txt >>datadir.txt
ifeq ($(CYGWIN),yes)
	u2d datadir.txt
endif 
echos:
	echo $(MKDIR)  $(ROOTDIR)$(MANDIR)
	echo $(CP) bed.html $(ROOTDIR)$(CONFDIR)/bed.html
	echo test -d /usr/man/man1  $(CP) bed.1 /usr/man/man1 
install:
	-$(MKDIR)  $(ROOTDIR)$(MANDIR)
ifeq ($(FULLUNIX),y)
ifeq ($(SYSTEM),Linux)
	$(CP) bed.1.gz $(ROOTDIR)$(MANDIR)/$(PROGRAM).1.gz
else
	$(CP) bed.1 $(ROOTDIR)$(MANDIR)/$(PROGRAM).1
endif
ifeq ($(CONFIG_XBED),y)
ifeq ($(SYSTEM),Linux)
	$(CP) xbed.1.gz $(ROOTDIR)$(MANDIR)/$(XPROGRAM).1.gz
else
	$(CP) xbed.1 $(ROOTDIR)$(MANDIR)/$(XPROGRAM).1
endif
endif
#	-chmod 0644 $(ROOTDIR)$(MANDIR)/$(XPROGRAM).1 $(ROOTDIR)$(MANDIR)/$(PROGRAM).1
ifeq ($(CROSS_COMPILING),)
	-man -F bed 2>/dev/null >&2
endif
else
	$(CP) bed.html $(ROOTDIR)$(CONFDIR)/bed.html
ifneq ($(ROOTDIR),)
	-$(MKDIR)  -p $(ROOTDIR)$(MANDIR)
	$(CP) bed.1 $(ROOTDIR)$(MANDIR)
else
	-(test -d $(ROOTDIR)/usr/share/man/man1 && $(CP) bed.1 $(ROOTDIR)/usr/share/man/man1 )|| (test -d $(ROOTDIR)/usr/man/man1 && $(CP) bed.1 $(ROOTDIR)/usr/man/man1 )

endif
endif

realclean:
	-rm bed.txt bed.1 bed.1.gz prochelp.tus bed.1.tus1 bed.1.tus2 bed.html.tus2  bed.html.tus1 bed.html menu.html datadir.txt google$(BUILDEXEEXT) bed.tus3.html

sections.tus: sections.sed menu.html
	sed -n -f  sections.sed menu.html > sections.tus
bed.1: bed.1.tus2 Makefile
	sed -e '{s/PROGRAM/$(PROGRAM)/g;s/VERSION/$(VERSION)/g;s/CONFDIRLINK/$(CONFDIRLINKER)/g;s/GLOBALCONFIG/$(GLOBALCONFIG)/g;s/PERSCONFIG/$(PERSCONFIG)/g;s/proconly \(\<[^ 	]*\>\)/\1/g;s/prockey \(\<[^ 	]*\>\)/Alt-t,d,\1/g; }' < bed.1.tus2 > bed.1
oldbed.html: bed.1 
	$(MAN2HTML) -M /cgi-bin/man2html bed.1|sed -e '{s/Content-type: text\/html//g;s/^.*Return to Main Contents.*$$//g;s/^Section: User Commands.*$$//g;s/<BODY>/<body text="#000000" bgcolor="#FFFFFF" link="#0000EF" vlink="#55188A" alink="#FF0000">/g;}' > oldbed.html 
#menu.html:bed.html.tus2 menu.html.instart menu.html.inend mkindex.sh menu.html.nahead trackingcode.txt
menu.html:bed.html.tus2 menu.html.instart menu.html.inend mkindex.sh menu.html.nahead 
	bash mkindex.sh
view: bed.1
	nroff -mandoc bed.1|less
bed.1.gz:bed.1
	-rm bed.1.gz
	gzip -c bed.1 > bed.1.gz
bed.1.tus1: bed.1.in2a bed.1.in2b options.sed 
	cat bed.1.in2a > bed.1.tus1
	(../src/bed -h||bed -h)|sed  -f options.sed  >> bed.1.tus1
	cat bed.1.in2b >> bed.1.tus1
bed.1.tus2: bed.1.in1 ../description bed.1.tus1 prochelp.tus bed.1.inend
	sed -f ../src/chhelp < bed.1.in1 > bed.1.tus2
	sed -e 's/^[	 ]*//g' < ../description >> bed.1.tus2
	sed -f ../src/chhelp < bed.1.tus1 >> bed.1.tus2
	cat prochelp.tus >> bed.1.tus2
	sed -f ../src/chhelp < bed.1.inend >> bed.1.tus2
bed.html.tus2: bed.html.tus1	html1.sed ../src/chhelp html3.sed $(MAN2HTML) 
	cat bed.html.tus1|sed -e '{s/PROGRAM/$(PROGRAM)/g;s/VERSION/$(VERSION)/g;s/CONFDIRLINK/$(CONFDIRLINKER)/g;s/GLOBALCONFIG/$(GLOBALCONFIG)/g;s/PERSCONFIG/$(PERSCONFIG)/g;}' |$(MAN2HTML)   -HXYHOSTYX -M QRPATHRQ |sed -f html1.sed |sed -f ../src/chhelp|sed -f html3.sed  > bed.html.tus2
	cat bed.html.intail >> bed.html.tus2
google: google.cc
	$(TOBUILDCXX) $(BUILDCXXFLAGS) $< -o $@

bed.html: bed.html.tus2 sections.tus 
	sed -f sections.tus < bed.html.tus2| sed 's/\(<[Aa][^>]*http[^>]*\)>/\1 TARGET="_top">/g' > bed.html
#	sed -f sections.tus < bed.html.tus2 > bed.tus3.html
#	./google bed.tus3.html trackingcode.txt > bed.html

bed.html.tus1: bed.1.in1 ../description bed.1.tus1 prochelp.tus bed.1.inend bed.html.sed
	cat bed.1.in1 > bed.html.tus1
	sed -e 's/^[	 ]*//g' < ../description >> bed.html.tus1
	cat  bed.1.tus1  >> bed.html.tus1
	cat ../src/helpdata.in ../plugins/examples/plughelp ../plugins/examples/plugman |sed -f bed.html.sed >> bed.html.tus1
	cat bed.1.inend >>bed.html.tus1

prochelp.tus: ../plugins/examples/plugman ../src/helpdata ../src/todoc.sed
	cat ../src/helpdata ../plugins/examples/plugman |sed -f ../src/todoc.sed  > prochelp.tus

../plugins/examples/plugman:
	$(MAKE) -C../plugins/examples/ plugman
../src/helpdata:
	$(MAKE) -C ../src helpdata

try:
	sed -n -e 's/hallo/this\n\
is\n\
een\n\
zin/gp'



../man2html/Makefile:
	(cd ../man2html; ./conf_script	Makefile)

../man2html/man2html: ../man2html/Makefile
	(cd ../man2html; $(MAKE))
