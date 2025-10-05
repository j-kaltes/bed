ifeq ($(findstring Windows,$(OS)),Windows)
OpSys=Windows
endif
include config 
include prognames.mk
#MAKE=gmake
ifneq ($(CONFIG_PLUGIN),y)
ifneq ($(filter disass.plug,$(LINKIN) $(LINKINDYN)),)
DISASS_USE=y
endif
endif

ifneq ($(HAS_RE_SEARCH),y)
REGEX=$(REGEX_DIR)/regex.o
else
REGEX=
endif



ifeq ($(CONFIG_PROGMAN),y)
all:  has_appl has_file  $(REGEX) $(CONFFILE) bed.menu 
else
all:  has_appl has_file  $(REGEX) $(CONFFILE) 

endif

ifeq ($(BIOSDISK),y)
	$(MAKE) -C biosdisk 
endif
	$(MAKE) -C utils
	$(MAKE) -C plugins/examples  inlinklist plughelp
	$(MAKE) -C src
ifeq ($(CONFIG_PLUGIN),y)
	$(MAKE) -C plugins/examples  give
else
ifeq ($(DISASS_USE),y)
	$(MAKE) -C plugins/examples  disass-use
endif
endif


ifeq ($(CONFIG_XBED),y)
	$(MAKE) -C x11 $(XPROGRAM)
endif
	$(MAKE) -C doc

BEDBASE=$(PROGRAM)-$(VERSION)
LSMFILE=$(PROGRAM)-$(VERSION).lsm
SPECFILE=$(PROGRAM)-$(VERSION).spec

ROOTDIR:=$(abspath $(DESTDIR))

root:
	echo $(ROOTDIR)

ifeq ($(CONFIG_XBED),y)
XBEDMAN=$(ROOTDIR)$(MANDIR)/xbed.1 $(ROOTDIR)$(BINDIR)/xbed
endif
ifeq ($(CONFIG_PROGMAN),y)
MENUBED=$(ROOTDIR)$(MENUPATH)/bed
endif

ifeq ($(SYSTEM),Linux)
BINDIST=$(ROOTDIR)$(CONFDIR) $(ROOTDIR)$(CONFDIRLINK) $(ROOTDIR)$(BINDIR)/bed $(ROOTDIR)$(MANDIR)/bed.1.gz   $(MENUBED) $(XBEDMAN)
else
BINDIST=$(ROOTDIR)$(CONFDIR) $(ROOTDIR)$(CONFDIRLINK) $(ROOTDIR)$(BINDIR)/bed $(ROOTDIR)$(MANDIR)/bed.1   $(MENUBED) $(XBEDMAN)
endif


has_appl:has_appl.c
	$(TOBUILDCC)  $< -o $@

HAS_APPLICATIONS := $(shell ./has_appl)

has:
	echo $(HAS_APPLICATIONS)
ifneq ($(HAS_APPLICATIONS),)
BINDIST+=$(ROOTDIR)$(HAS_APPLICATIONS)/bed-binary-editor.desktop 
endif
ifeq ($(SYSTEM),FreeBSD)
BINDIST+=$(ROOTDIR)/usr/local/lib/nss_dns.so.1 
endif

#SORT=LC_ALL=C sort
install: installfiles
ifeq ($(DESTDIR),)
ifeq ($(HAS_FILE),y)
	@bash ./addmagic
endif
ifeq ($(CONFIG_PROGMAN),y)
	update-menus
endif
endif

installfiles: $(CONFFILE)  #$(LSMFILE)
ifeq ($(WINDOWS),yes)
	-cd "c:\\"
endif
	-$(MKDIR) $(ROOTDIR)$(BINDIR)
	-rm   $(ROOTDIR)$(BINDIR)/$(EXEPROGRAM)
	$(CP) src/$(EXEPROGRAM) $(ROOTDIR)$(BINDIR)
	$(STRIP)  $(ROOTDIR)$(BINDIR)/$(EXEPROGRAM)
	-rm   $(ROOTDIR)$(CONFDIRLINK)
	-mv -f  $(ROOTDIR)$(CONFDIRLINK) $(ROOTDIR)$(CONFDIRLINK).bak
	-$(MKDIR) $(ROOTDIR)$(CONFDIR)
ifneq ($(WINDOWS),yes)
	$(MKDIR) `dirname $(ROOTDIR)$(CONFDIRLINK)`
	-mv  $(ROOTDIR)$(CONFDIR)/$(CONFFILE) $(ROOTDIR)$(CONFDIR)/~$(CONFFILE)
endif
	$(CP) $(CONFFILE) $(ROOTDIR)$(CONFDIR)
	grep  BUILDEXEEXT build > $(ROOTDIR)$(CONFDIR)/build
	echo "HAS_FILE := $(shell $(BEDSRCDIR)/has_file)" >> $(ROOTDIR)$(CONFDIR)/build
	chmod 0644 	$(ROOTDIR)$(CONFDIR)/$(CONFFILE)
	$(MAKE) -C doc install ROOTDIR=$(ROOTDIR)
	$(MAKE) -C misc install ROOTDIR=$(ROOTDIR)
ifeq ($(CONFIG_XBED),y)
	$(MAKE) -C x11 install ROOTDIR=$(ROOTDIR)
endif
ifeq ($(CONFIG_PLUGIN),y)
	$(MAKE) -C utils install ROOTDIR=$(ROOTDIR)
	$(MAKE) -C plugins install ROOTDIR=$(ROOTDIR)

else
ifeq ($(DISASS_USE),y)
	$(CP) plugins/examples/disass-use*  $(ROOTDIR)$(CONFDIR)
endif
endif
ifneq ($(WINDOWS),yes)
ifeq ($(CONFIG_PLUGIN),y)
ifeq ($(HAVE_LN_R),n)
	-$(SYMLINK)   -s $(BINDIR)/$(PROGRAM) $(ROOTDIR)$(CONFDIR)/utils/keyconfig
else
	-$(SYMLINK) -r  -s $(ROOTDIR)$(BINDIR)/$(PROGRAM) $(ROOTDIR)$(CONFDIR)/utils/keyconfig
endif
else
ifeq ($(HAVE_LN_R),n)
	-$(SYMLINK)  -s $(BINDIR)/$(PROGRAM) $(ROOTDIR)$(CONFDIR)/keyconfig
else
	-$(SYMLINK) -r -s $(ROOTDIR)$(BINDIR)/$(PROGRAM) $(ROOTDIR)$(CONFDIR)/keyconfig
endif
endif
else
ifeq ($(BIOSDISK),y)
	$(MAKE) -C biosdisk install ROOTDIR=$(ROOTDIR)
endif
ifeq ($(PDCURSES),)
	echo $(EXEPROGRAM) -k %1 %2 %3 %4 %5 %6 %7 %8 %9 > $(ROOTDIR)$(CONFDIR)/keyconfig.bat
endif
endif
ifeq ($(PDCURSES),)
#	$(MKDIR) $(ROOTDIR)$(CONFDIR)/terminfo
#	$(MKDIR) $(ROOTDIR)$(CONFDIR)/terminfo/r
#	cp   terminfo/r/rxvt* $(ROOTDIR)$(CONFDIR)/terminfo/r
	cp -a  terminfo $(ROOTDIR)$(CONFDIR)
endif
	cp    LICENSE $(ROOTDIR)$(CONFDIR)
ifeq ($(OpSys),Windows)
	u2d     $(ROOTDIR)$(CONFDIR)/LICENSE
endif
	cp doc/datadir.txt $(ROOTDIR)$(CONFDIR)/README
	cp public.pgp $(ROOTDIR)$(CONFDIR)
ifeq ($(SYSTEM),Linux)
	cp bedkeys $(ROOTDIR)$(CONFDIR)
endif
ifeq ($(SYSTEM),FreeBSD)
	cp us.alt.kbd README.FreeBSD $(ROOTDIR)$(CONFDIR)
endif
ifeq ($(SYSTEM),SunOS)
	cp README.OpenSolaris $(ROOTDIR)$(CONFDIR)
endif
ifeq ($(HAS_FILE),y)
	$(CP) testmarks.bedm bedmagic bedmagic.short addmagic $(ROOTDIR)$(CONFDIR)
endif
ifneq ($(HAS_APPLICATIONS),)
	$(MKDIR) $(ROOTDIR)$(HAS_APPLICATIONS)
	cp progman/bed-binary-editor.desktop $(ROOTDIR)$(HAS_APPLICATIONS)/bed-binary-editor.desktop 
endif
ifeq ($(CONFIG_PROGMAN),y)
#	cp bed.xpm $(ROOTDIR)$(ICONPATH)
#	cp bedsmall.xpm $(ROOTDIR)$(ICONPATH)/mini/bed.xpm
	$(MKDIR) $(ROOTDIR)$(MENUPATH)
	cp bed.menu $(ROOTDIR)$(MENUPATH)/$(PROGRAM)
	chmod 0644 $(ROOTDIR)$(MENUPATH)/$(PROGRAM)

#	@rpm -q $(PROGRAM)|| echo Warning!! Desktop entries not created if no package $(PROGRAM) exists
endif
ifneq ($(WINDOWS),yes)

ifeq ($(SYSTEM),FreeBSD)
	$(SYMLINK) -r -f $(ROOTDIR)$(CONFDIR) $(ROOTDIR)$(CONFDIRLINK)
else
ifeq ($(HAVE_LN_R),n)
	$(SYMLINK) -T -f  $(CONFDIR) $(ROOTDIR)$(CONFDIRLINK)
else
	$(SYMLINK) -r -T -f  $(ROOTDIR)$(CONFDIR) $(ROOTDIR)$(CONFDIRLINK)
endif
endif
endif
ifeq ($(SYSTEM),Android)
	-$(MKDIR) $(ROOTDIR)$(BASEDIR)/tmp
	-chmod 01777 $(ROOTDIR)$(BASEDIR)/tmp
endif
ifneq ($(WINDOWS),yes)
	@cp uninstallhead $(ROOTDIR)$(CONFDIR)/uninstallbed.sh
	@FILES=`for afile in $(BINDIST); do  test -e $$afile -o -L $$afile&&echo $$afile ;done`;if test . != .$(ROOTDIR); then SLASHROOT=`echo $(ROOTDIR)| sed -e 's/\\//\\\\\\//g'`;fi;find  $$FILES ! -type d -print |if test . != .$(ROOTDIR);then sed -e "s/$$SLASHROOT//g";else cat;fi |xargs echo $(REMOVE) ' ' >> $(ROOTDIR)$(CONFDIR)/uninstallbed.sh ;find  $$FILES -type d  -a  -print| if test . != .$(ROOTDIR);then sed -e "s/$$SLASHROOT//g";else cat;fi |$(SORT) -r|xargs echo $(RMDIR) ' ' >> $(ROOTDIR)$(CONFDIR)/uninstallbed.sh
	@echo Uninstall with sh $(ROOTDIR)$(CONFDIR)/uninstallbed.sh
endif
	@echo Binary: '$(ROOTDIR)$(BINDIR)/$(EXEPROGRAM)'
	@echo Support files: $(ROOTDIR)$(CONFDIR)



cleanhere:
	-rm  $(REGEX_DIR)/regex.o hosttest$(BUILDEXEEXT) has_appl$(BUILDEXEEXT) has_file$(BUILDEXEEXT)
clean: cleanhere
	$(MAKE) -C src clean
	$(MAKE) -C utils clean
	$(MAKE) -C plugins clean
	$(MAKE) -C x11 clean
	$(MAKE) -C biosdisk clean
	-$(MAKE) -C doc clean
	-$(MAKE) -C man2html clean


realclean: cleanhere
	-rm $(CONFFILE)  config.cache config.bak config.log config.status  bedrc buildonce getsize
	 $(MAKE) -C x11 clean
	$(MAKE) -C src realclean
	- $(MAKE) -C doc realclean
	- $(MAKE) -C man2html clean
	 $(MAKE) -C utils realclean
	 $(MAKE) -C plugins realclean
	 $(MAKE) -C biosdisk realclean
	-rm  $(REGEX_DIR)/regex.o


echo:
	echo $(BINDIST)
.PHONY: %.dist %.bindist %.srcdist %.html
HTMLPATH=../html
DISTPATH=$(HTMLPATH)
TGZSUF=.tar.xz
SRCBASE=$(PROGRAM)-$(VERSION).src
SRCARGFILE=$(SRCBASE)$(TGZSUF)
SRCLSMFILE=$(SRCBASE).lsm
SRCASCFILE=$(SRCBASE).asc
#s/$(TGZSUF)/$(TGZSUF)/g


SOURCEFILE=$(DISTPATH)/download/$(PROGRAM)-$(VERSION).src$(TGZSUF) 
ifeq ($(CONFIG_STATIC),y)
ELFBASE=$(PROGRAM)-$(VERSION).$(SYSTEM).ELF-static$(ELFSIZE)
else
#ELFBASE=$(PROGRAM)_$(VERSION)-$(SYSTEM)
ELFBASE=$(PROGRAM)-$(VERSION).$(SYSTEM)
endif
ELFARGFILE=$(ELFBASE)$(ELFSIZE)$(TGZSUF) 
ELFLSMFILE=$(ELFBASE).lsm
ELFASCFILE=$(ELFBASE).asc
WIN32ARGFILE=$(PROGRAM)-$(VERSION).win32.zip
WIN95ARGFILE=$(PROGRAM)-$(VERSION).win95.zip
#NWIN32ARGFILE=$(PROGRAM)-$(VERSION).win32ncurses.zip
NWIN32ARGFILE=$(PROGRAM)-$(VERSION).win32ncuronly.zip

WIN64ARGFILE=$(PROGRAM)-$(VERSION).win64.zip
#NWIN64ARGFILE=$(PROGRAM)-$(VERSION).win64ncuronly.zip
NWIN64ARGFILE=$(PROGRAM)-$(VERSION).win64ncurses.zip


FREEBSDARGFILE=$(PROGRAM)-$(VERSION).FreeBSD64.tbz
#FREEBSDARGFILE=$(PROGRAM)-0.2.22.FreeBSD64$(TGZSUF)
#FREEBSDARGFILE=$(PROGRAM)-0.2.22.FreeBSD64$(TGZSUF)
OPENSOLARISFILE=$(PROGRAM)-$(VERSION).SunOS.tar.xz
STATICARGFILE=$(PROGRAM)-$(VERSION).bin.ELF-static$(TGZSUF)
#NONSTATICARGFILE32=$(PROGRAM)-$(VERSION).Linux32$(TGZSUF)
#NONSTATICARGFILE64=$(PROGRAM)-$(VERSION).Linux64$(TGZSUF)
NONSTATICARGFILE64=$(PROGRAM)_$(VERSION)_amd64.deb
NONSTATICARGFILE32=$(PROGRAM)_$(VERSION)_i386.deb
NONSTATICARGFILE_ARMHF=$(PROGRAM)_$(VERSION)_armhf.deb
ANDROID64ARGFILE=$(PROGRAM)-$(VERSION).arm64.android8.0.tar.gz
ANDROIDARGFILE=$(ANDROID64ARGFILE)
#ANDROIDARGFILE=$(PROGRAM)-$(VERSION)-arm-android-17.tar.gz
RPMFILE=bed-$(VERSION)-1.i686.rpm

#LSMFILE=$(PROGRAM)-$(VERSION).lsm
FULLELFFILE=$(DISTPATH)/download/$(ELFARGFILE)
#PGPSIGNPROG=gpg -s --use-embedded-filename -a -b -u 0x94BA03AE 
#PGPSIGNPROG=gpg2 -s --use-embedded-filename -b -u 0x94BA03AE 

#PGPSIGNPROG=gpg2 -s   -b -u 0x94BA03AE 

# --use-embedded-filename

#  --comment string

#       --no-comments

#--clearsign


#PGPSIGNPROG= pgps -a -b -u 0x94BA03AE 

PGPSIGNPROG=gpg --cert-digest-algo=SHA256 -s -a  -b -u 0x94BA03AE -u 0x65D385A5 

mkuninstall:
	@cp uninstallhead $(CONFDIR)/uninstallbed.sh
	@FILES=`for afile in $(BINDIST); do  test -e $$afile -o -L $$afile&&echo $$afile ;done`; find  $$FILES ! -type d|xargs echo $(REMOVE) ' ' >> $(CONFDIR)/uninstallbed.sh ;find  $$FILES -type d|$(SORT) -r|xargs echo $(RMDIR) ' ' >> $(CONFDIR)/uninstallbed.sh
#	find  $(BINDIST) -type f|xargs 'echo rm -f '
#	find  $(BINDIST) -type d|$(SORT) -r|xargs 'echo rmdir '

%.bindist: $(BINARIES)
	mkdir -p $*/download
	tar --use-compress-program="xz -ze" -cf $*/download/$(ELFARGFILE)   $(BINDIST) 
#	env VERSION=$(VERSION) ELFBASE=$(ELFBASE) SRCBASE=$(SRCBASE) DISTPATH=$*/download sh ./mklsmelf > $*/download/$(ELFLSMFILE)
%.srcdist:
	mkdir -p $*/download
	tar --use-compress-program="xz -ze" -cf  $*/download/$(PROGRAM)-$(VERSION).src$(TGZSUF)  -C ../  $(PROGRAM)-$(VERSION) 
#	env  ELFBASE=$(ELFBASE) SRCBASE=$(SRCBASE) VERSION=$(VERSION) DISTPATH=$*/download bash ./mklsm > $*/download/$(SRCLSMFILE)
%.html: mkbed 
	mkdir -p $*
	$(MAKE) -C doc bed.html
	cp doc/bed.html $*/bed.1.html
	$(MAKE) -C doc menu.html
	env SRCARGFILE=$(SRCARGFILE) WIN32ARGFILE=$(WIN32ARGFILE)  WIN95ARGFILE=$(WIN95ARGFILE)  OPENSOLARISFILE=$(OPENSOLARISFILE) FREEBSDARGFILE=$(FREEBSDARGFILE) NONSTATICARGFILE32=$(NONSTATICARGFILE32) ANDROIDARGFILE=$(ANDROIDARGFILE) NONSTATICARGFILE_ARMHF=$(NONSTATICARGFILE_ARMHF) NONSTATICARGFILE64=$(NONSTATICARGFILE64) STATICARGFILE=$(STATICARGFILE) ELFBASE=$(ELFBASE) SRCBASE=$(SRCBASE) VERSION=$(VERSION) DISTPATH=$*/download bash ./mkbed | sh dseonlyext.sh >$*/download.html
	sh dseext.sh <$*/download.html > $*/downloaddse.html
	env  BEDVERSION=$(VERSION) LSMFILE=$(LSMFILE)  ELFBASE=$(ELFBASE) SRCBASE=$(SRCBASE) VERSION=$(VERSION) DISTPATH=$*/download bash ./mkoldpage.sh > $*/oldbeds.html
	cp doc/menu.html $*/menu.html

#	env  WIN32ARGFILE=$(WIN32ARGFILE)  OPENSOLARISFILE=$(OPENSOLARISFILE) FREEBSDARGFILE=$(FREEBSDARGFILE) NONSTATICARGFILE32=$(NONSTATICARGFILE32) NONSTATICARGFILE64=$(NONSTATICARGFILE64) STATICARGFILE=$(STATICARGFILE) ELFBASE=$(ELFBASE) SRCBASE=$(SRCBASE) VERSION=$(VERSION) DISTPATH=$*/download bash ./mknewlsm > $*/download/$(LSMFILE)
%.dist: %.srcdist %.bindist %.sign
	echo $*




#s/FREEBSDARGFILE/OPENSOLARISFILE/g

%.sign:
	-$(PGPSIGNPROG)   --comment $(SRCARGFILE) $*/download/$(SRCARGFILE) 
	-$(PGPSIGNPROG)   --comment $(WIN64ARGFILE) $*/download/$(WIN64ARGFILE) 
	-$(PGPSIGNPROG)   --comment $(ANDROIDARGFILE) $*/download/$(ANDROIDARGFILE) 
	-$(PGPSIGNPROG)   --comment $(NWIN64ARGFILE) $*/download/$(NWIN64ARGFILE) 
	-$(PGPSIGNPROG)   --comment $(WIN32ARGFILE) $*/download/$(WIN32ARGFILE) 
	-$(PGPSIGNPROG)   --comment $(NWIN32ARGFILE) $*/download/$(NWIN32ARGFILE) 
	-$(PGPSIGNPROG)   --comment $(NONSTATICARGFILE32) $*/download/$(NONSTATICARGFILE32) 
	-$(PGPSIGNPROG)   --comment $(NONSTATICARGFILE64) $*/download/$(NONSTATICARGFILE64) 
	-$(PGPSIGNPROG)   --comment $(NONSTATICARGFILE_ARMHF) $*/download/$(NONSTATICARGFILE_ARMHF) 
#	make $*/download/$(LSMFILE) 

#	$(PGPSIGNPROG)  $*/download/$(STATICARGFILE)
#	env  WIN32ARGFILE=$(win32argfILE)  OPENSOLARISFILE=$(OPENSOLARISFILE) FREEBSDARGFILE=$(FREEBSDARGFILE) NONSTATICARGFILE32=$(NONSTATICARGFILE32) NONSTATICARGFILE64=$(NONSTATICARGFILE64) STATICARGFILE=$(STATICARGFILE) ELFBASE=$(ELFBASE) SRCBASE=$(SRCBASE) VERSION=$(VERSION) DISTPATH=$*/download bash ./mknewlsm > $*/download/$(LSMFILE)

%.lsm: ./settings
	env SRCARGFILE=$(SRCARGFILE) WIN32ARGFILE=$(WIN32ARGFILE)  WIN95ARGFILE=$(WIN95ARGFILE)  OPENSOLARISFILE=$(OPENSOLARISFILE) FREEBSDARGFILE=$(FREEBSDARGFILE) NONSTATICARGFILE32=$(NONSTATICARGFILE32) NONSTATICARGFILE64=$(NONSTATICARGFILE64) STATICARGFILE=$(STATICARGFILE) ELFBASE=$(ELFBASE) SRCBASE=$(SRCBASE) VERSION=$(VERSION) DISTPATH=../html/download bash ./mknewlsm > $*.lsm
#	env  WIN32ARGFILE=$(WIN32ARGFILE)  FREEBSDARGFILE=$(FREEBSDARGFILE) NONSTATICARGFILE=$(NONSTATICARGFILE)  STATICARGFILE=$(STATICARGFILE) ELFBASE=$(ELFBASE) SRCBASE=$(SRCBASE) VERSION=$(VERSION) DISTPATH=../html/download bash ./mknewlsm > $(LSMFILE)
	echo $*

.PHONY: lsm

lsm: 	
	-rm $(LSMFILE)
	$(MAKE) $(LSMFILE)
ifeq ($(OpSys),Windows)
	u2d  $(LSMFILE)
endif



BEDTMP=c:/tmp/bed-$(VERSION)

MIXBINDIR=`cygpath -m $(BINDIR)`
pdir:
	echo var $(PWD)
	cd "c:\\"; ls;pwd; echo var $(PWD)
winbindist:
	-rm -i ../html/download/bed-$(VERSION).win32.zip
ifeq ($(CYGWIN),no)
	cd $(ROOTDIR)$(prefix);(zip  -z -r $(PWD)/../html/download/bed-$(VERSION).win64.zip   Program\ Files/bed  Windows/$(EXEPROGRAM) 'ProgramData/Menu Start/bed.lnk' 'Windows/Start Menu/bed.lnk' < $(PWD)/bedmingw.txt)
else
	cd $(ROOTDIR)$(prefix); (zip  -z -r $(PWD)/../html/download/bed-$(VERSION).win64.zip   Program\ Files/bed Windows/cygwin1.dll Windows/$(EXEPROGRAM) 'ProgramData/Menu Start/bed.lnk' 'Windows/Start Menu/bed.lnk' < $(PWD)/bedcygwin.txt)
endif
	bash $(PWD)/../html/mkbedpad.sh $(VERSION)

#	zip  -z -r $(PWD)/../html/download/bed-$(VERSION).win32.zip $(PWD)/bedwin32.txt  "`cygpath -m $(CONFDIR)`" $(MIXBINDIR)/cygwin1.dll $(MIXBINDIR)/$(EXEPROGRAM) '$(ALLUSERSPROFILE)/Menu Start/bed.lnk' 'c:/windows/Start Menu/bed.lnk' < bedwin32.txt

#CYGWIN=no
nwinbindist:
	-rm -i bed-$(VERSION).win64.zip 
	zip  -z -r bed-$(VERSION).win64.zip bedwin32.txt $(BINDIR)/cygwin1.dll  $(BINDIR)/$(EXEPROGRAM)  $(BINDIR)/bedn.exe $(CONFDIR) < bedwin32.txt

	
#	-mkdir $(BEDTMP)
#	cp  $(BINDIR)/$(EXEPROGRAM) $(BEDTMP)
#	cp  $(CONFDIR)/misc/*.bedda $(BEDTMP)
#	cp  $(CONFDIR)/doc/bed* $(BEDTMP)
#	cp  $(CONFDIR)/* $(BEDTMP)
#	cd $(BEDTMP); pkzip  -r bedzip *.*

#	pkzip  -r bedzip $(BINDIR)/$(EXEPROGRAM) $(CONFDIR)
#	find $(CONFDIR) ! -type f -a -printf \%p\  -a


#	tar -cf bedtar $(BINDIR)/$(EXEPROGRAM) $(CONFDIR)
#	gzip  bedtar
#	mv be$(TGZSUF) bed-$(VERSION).win32$(TGZSUF)

#	tar -cf bedtar $(BINDIR)/$(EXEPROGRAM) $(CONFDIR)
#	gzip  bedtar
#	mv be$(TGZSUF) bed-$(VERSION).win32$(TGZSUF)

#	mv bedwin32.zip bed-$(VERSION).win32.zip
#	zip -9 -r bedwin32.zip  $(BINDIR)/$(EXEPROGRAM) $(CONFDIR)
#	mv bedwin32.zip bed-$(VERSION).win32.zip




sign: $(DISTPATH).sign 
dist: $(DISTPATH).dist
srcdist: $(DISTPATH).srcdist
bindist: $(DISTPATH).bindist
html:$(DISTPATH).html
files:
	tar -a -tf $(FULLELFFILE)|sed -e 's/^/\//'> files
spec:  files
	PROGMANEXTRA= MENUPATH=$(MENUPATH) VERSION=$(VERSION) DISTPATH=$(HTMLPATH)/download HAS_APPLICATIONS=$(HAS_APPLICATIONS) bash ./mkspec > $(PROGRAM)-$(VERSION).spec
solspec:  files
	PROGMANEXTRA= MENUPATH=$(MENUPATH) VERSION=$(VERSION) DISTPATH=$(HTMLPATH)/download HAS_APPLICATIONS=$(HAS_APPLICATIONS) bash ./mksolspec > $(PROGRAM)-$(VERSION).spec
.PHONY: control
control:
	ICONPATH=$(ICONPATH) MENUPATH=$(MENUPATH) KDEMENU=$(KDEMENU) GNOMEMENU=$(GNOMEMENU) XBEDMENU=$(XBEDMENU) BEDMENU=$(BEDMENU) VERSION=$(VERSION) DISTPATH=$(HTMLPATH)/download bash debian/control.sh > debian/control
	CONFDIR=$(CONFDIR) ICONPATH=$(ICONPATH) MENUPATH=$(MENUPATH) KDEMENU=$(KDEMENU) GNOMEMENU=$(GNOMEMENU) XBEDMENU=$(XBEDMENU) BEDMENU=$(BEDMENU) VERSION=$(VERSION) DISTPATH=$(HTMLPATH)/download bash debian/postinst.sh > debian/postinst
	CONFDIR=$(CONFDIR) ICONPATH=$(ICONPATH) MENUPATH=$(MENUPATH) KDEMENU=$(KDEMENU) GNOMEMENU=$(GNOMEMENU) XBEDMENU=$(XBEDMENU) BEDMENU=$(BEDMENU) VERSION=$(VERSION) DISTPATH=$(HTMLPATH)/download bash debian/preinst.sh > debian/preinst
	CONFDIR=$(CONFDIR) ICONPATH=$(ICONPATH) MENUPATH=$(MENUPATH) KDEMENU=$(KDEMENU) GNOMEMENU=$(GNOMEMENU) XBEDMENU=$(XBEDMENU) BEDMENU=$(BEDMENU) VERSION=$(VERSION) DISTPATH=$(HTMLPATH)/download bash debian/postrm.sh > debian/postrm
mount:
	-mount /mnt/floppy
flop: mount /mnt/floppy.dist


#RPMROOT=/usr/src/redhat
#RPMROOT=/usr/src/RPM
RPMROOT=~/rpmbuild
#RPM = rpm
RPM = rpmbuild
#RPMFLAGS =  --sign -ba
RPMFLAGS =  -ba

rpm: spec
	$(MKDIR) $(RPMROOT)/SOURCES
	cp $(SOURCEFILE) $(RPMROOT)/SOURCES/$(PROGRAM)-$(VERSION).src$(TGZSUF)
	$(MKDIR) $(RPMROOT)/SPECS
	cp  $(PROGRAM)-$(VERSION).spec $(RPMROOT)/SPECS/$(PROGRAM)-$(VERSION).spec 
	$(RPM) $(RPMFLAGS) $(PROGRAM)-$(VERSION).spec 

PKGROOT=/export/home/jka/packages
#RPM = rpm
PKG = pkgbuild
#RPMFLAGS =  --sign -ba
PKGFLAGS =  -ba
sunos: solspec
	$(MKDIR) $(PKGROOT)/SOURCES
	cp $(SOURCEFILE) $(PKGROOT)/SOURCES/$(PROGRAM)-$(VERSION).src$(TGZSUF)
	$(MKDIR) $(PKGROOT)/SPECS
	cp  $(PROGRAM)-$(VERSION).spec $(PKGROOT)/SPECS/$(PROGRAM)-$(VERSION).spec 
	$(PKG) $(PKGFLAGS) $(PROGRAM)-$(VERSION).spec 

package: Makefile
	$(RPM) --short-circuit $(RPMFLAGS) $(PROGRAM)-$(VERSION).spec 

installrpm:
	$(RPM) --short-circuit  --sign  -bi $(PROGRAM)-$(VERSION).spec 


ifneq ($(WINDOWS),yes)
CONFFILESH=mkbedrc.sh
else
CONFFILESH=mkbedrc95.sh
endif
$(CONFFILE):$(CONFFILESH) plugins/examples/Makefile config
	$(MAKE) -C plugins/examples ingive
	env INLINKALL="$(INLINKALL)" HAS_X=$(HAS_X) CONFIG_LINKINPLUGINS=$(CONFIG_LINKINPLUGINS) CONFIG_PLUGIN=$(CONFIG_PLUGIN) CONFIG_THREADS=$(CONFIG_THREADS) PDCURSES=$(PDCURSES) CONFDIRLINK=$(CONFDIRBEDVERS) CONFDIR=$(CONFDIR) bash $(CONFFILESH)> $(CONFFILE) 
ifeq ($(WINDOWS),yes)
	u2d $(CONFFILE)
endif
#s/\$(CONFFILESH)/mkbedrc.sh/g
unixbedrc:$(CONFFILESH) plugins/examples/Makefile config
	$(MAKE) -C plugins/examples ingive
	env HAS_X=$(HAS_X) CONFIG_LINKINPLUGINS=$(CONFIG_LINKINPLUGINS) CONFIG_PLUGIN=$(CONFIG_PLUGIN) CONFIG_THREADS=$(CONFIG_THREADS) PDCURSES=$(PDCURSES) CONFDIRLINK=$(CONFDIRBEDVERS) CONFDIR=$(CONFDIR) bash mkbedrc.sh> unixbedrc
ifeq ($(WINDOWS),yes)
	u2d unixbedrc
endif

DATE=`date +%M-%H-%d-%m-%Y`

backup: 
	date > ../backup/backupfile.start
	tar --use-compress-program="xz -ze" -cf ../backup/$(PROGRAM)-$(VERSION).backup.tar.xz-$(DATE) -N `cat ../backup/backupfile` `find . ! -name *.o* -a ! -name core -a ! -name *.plug -a ! -type d -a ! -path ./src/$(PROGRAM) -a -printf %p `
	move  ../backup/backupfile.start ../backup/backupfile
fullbackup: 
	date > ../backup/backupfile.start
	tar --use-compress-program="xz -ze" -cf ../backup/$(PROGRAM)-$(VERSION).backup.tar.xz-$(DATE) `find . ! -name *.o* -a ! -name core -a ! -name *.plug -a ! -type d -a ! -path ./src/$(PROGRAM) -a -printf %p `
	move  ../backup/backupfile.start ../backup/backupfile
try:
	 echo $(TESTVAR)

./configure:./configure.ac src/defines.h.in
	autoconf


uninstall:
ifeq ($(CONFIG_PROGMAN),y)
	-rm  -f $(ROOTDIR)$(MENUPATH)/$(PROGRAM)
endif
ifneq ($(HAS_APPLICATIONS),)
	-rm  -f $(ROOTDIR)$(HAS_APPLICATIONS)/bed-binary-editor.desktop
endif
ifeq ($(CONFIG_XBED),y)
	-rm -f $(BINDIR)/$(XPROGRAM)
	-rm  -f $(MANDIR)/$(XPROGRAM).1
endif
	-rm  -f $(CONFDIRLINK)
	-rm -f $(BINDIR)/$(EXEPROGRAM)
	-rm  -f $(MANDIR)/$(PROGRAM).1
	-rm -fr $(CONFDIR)

bed.menu:mkmenu.sh config
	CONDIRLINK=$(CONFDIR) MENUNAME=$(MENUNAME) XMENUNAME=$(XMENUNAME) BEDMENU=$(BEDMENU) XBEDMENU=$(XBEDMENU) BINDIR=$(BINDIR) PROGRAM=$(PROGRAM)  XPROGRAM=$(XPROGRAM) bash ./mkmenu.sh > bed.menu

cprpm:
	cp /usr/src/redhat/RPMS/i386/bed-$(VERSION)*.rpm ../html/download
	cp /usr/src/redhat/SRPMS/bed-$(VERSION)*src*.rpm ../html/download


CONFDIRLINKER=$(shell echo '$(CONFDIRLINK)'| sed -e 's/\//\\\//g' )
README: .start.txt .common.txt
	cat .start.txt > README
	sed 's/CONFDIRLINK/$(CONFDIR)/g' < .common.txt >>README
	
pkg/pkg-plist:
	-rm $@
	@FILES=`for afile in $(BINDIST); do  test -e $$afile&&echo $$afile ;done`;PREFIXX=`echo $(prefix)|sed -e 's/\\//\\\\\\//g' `;find  $$FILES ! -type d ! -name *bed.1 -print | sed -e s/$$PREFIXX'\///g'|$(SORT) >> $@  ;find  $$FILES -type d  -a  -print |$(SORT) -r|sed -e s/$$PREFIXX'\//\@dirrm\ /g' >> $@ 
#	echo '@unexec rm %D/share/bed' >> $@
#;echo $(CONFDIRLINK) |sed -e s/$$PREFIXX'\///g'>>$@
#sed -e 's/$$PREFIXX/\@dirrm/g'

./has_file: has_file.c
	$(TOBUILDCC) has_file.c -o has_file

plugins/examples/inlinklist:
	$(MAKE) -C plugins/examples makeinlinklist

dep: ./has_appl ./has_file plugins/examples/inlinklist
	$(MAKE) -C src mainprocedures.h
	$(MAKE) -C plugins/examples dep
	$(MAKE) -C src dep

undos:
	file `find .	 -type f`|grep CRLF|cut -d : -f 1|xargs d2u

debug: 
	bash ./configure 
	mv ./config ./config.bak
	sed -e '{s/^CONFIG_DEBUG=.*/CONFIG_DEBUG=m/g;s/^CONFIG_MMAP=.*/CONFIG_MMAP=n/g;}' < ./config.bak > ./config
linuxdebug: 
	bash ./configure --prefix=/usr
	mv ./config ./config.bak
	sed -e '{s/^CONFIG_DEBUG=.*/CONFIG_DEBUG=m/g;s/^CONFIG_MMAP=.*/CONFIG_MMAP=n/g;s/^prefix=.*/prefix=\/usr/g;}' < ./config.bak > ./config
	echo '#define DLDEFAULTCOLORS 1' >> src/confdefines.h
linuxconfig: 
	bash ./configure --prefix=/usr
	mv ./config ./config.bak
	sed -e '{s/^CONFIG_DEBUG=.*/CONFIG_DEBUG=n/g;s/^prefix=.*/prefix=\/usr/g;}' < ./config.bak > ./config
#	sed -e '{s/^CONFIG_DEBUG=.*/CONFIG_DEBUG=n/g;s/^CONFIG_MMAP=.*/CONFIG_MMAP=y/g;s/^prefix=.*/prefix=\/usr/g;}' < ./config.bak > ./config
	echo '#define DLDEFAULTCOLORS 1' >> src/confdefines.h
#	echo '#define NCURSESLIB libncurses.so.5' >> src/defines.h
staticconfig: 
	bash ./configure
	mv ./config ./config.bak
	sed -e '{s/^CONFIG_STATIC=.*/CONFIG_STATIC=y/g;s/^CONFIG_DEBUG=.*/CONFIG_DEBUG=n/g;s/^CONFIG_MMAP=.*/CONFIG_MMAP=y/g;s/^prefix=.*/prefix=\/usr/g;}' < ./config.bak > ./config
	echo '#define DLDEFAULTCOLORS 1' >> src/confdefines.h

nwinconfig: 
	echo 'CURSESLIB=ncurses' >> settings
	bash ./configure
	mv ./config ./config.bak
	sed -e 's/^CONFIG_DEBUG=.*/CONFIG_DEBUG=n/g' < ./config.bak > ./config
nwindebugconfig: 
	echo 'CURSESLIB=ncurses' >> settings
	bash ./configure
	mv ./config ./config.bak
	sed -e 's/^CONFIG_DEBUG=.*/CONFIG_DEBUG=m/g' < ./config.bak > ./config
nwindebug: realclean nwindebugconfig dep all
	
winconfig: 
	echo 'CURSESLIB=pdcurses' >> settings
	bash ./configure
	mv ./config ./config.bak
	sed -e '{s/^CONFIG_STATIC=.*/CONFIG_STATIC=y/g;s/^HAVE_MAGIC_H=.*/HAVE_MAGIC_H=0/g;s/^CONFIG_DEBUG=.*/CONFIG_DEBUG=n/g;s/LINKINDYN.*file.plug/#&/g;}' < ./config.bak > ./config
androidconfig: 
	echo 'CURSESLIB=ncurses' >> settings
	echo 'CPPFLAGS+=-D__ANDROID_API__=17' >> settings
	bash ./configure --host=arm-linux-androideabi --prefix=/data/local
	mv ./config ./config.bak
	sed -e '{s/^HAVE_MAGIC_H=.*/HAVE_MAGIC_H=0/g;s/^CONFIG_DEBUG=.*/CONFIG_DEBUG=n/g;s/^CONFIG_PLUGIN=.*/CONFIG_PLUGIN=n/g;s/^UNIXPLUGINS=.*/UNIXPLUGINS=n/g;s/LINKINDYN+=index2table.plug//g;s/LINKINDYN+=elftables32.plug//g;}' < ./config.bak > ./config
windebugconfig2: 
	echo 'CURSESLIB=pdcurses' >> settings
	bash ./configure
	mv ./config ./config.bak
	sed -e '{s/^CONFIG_DEBUG=.*/CONFIG_DEBUG=m/g;s/LINKINDYN.*file.plug/#&/g;}' < ./config.bak > ./config
windebugconfig: 
	echo 'CURSESLIB=pdcurses' >> settings
	bash ./configure
	mv ./config ./config.bak
	sed -e '{s/^#export BIOSDISK=y/export BIOSDISK=y/g;s/^CONFIG_DEBUG=.*/CONFIG_DEBUG=m/g;s/LINKINDYN.*file.plug/#&/g;}' < ./config.bak > ./config

tryconfig: 
	mv ./config ./config.bak
	sed -e '{s/^#export BIOSDISK=y/export BIOSDISK=y/g;s/^CONFIG_DEBUG=.*/CONFIG_DEBUG=m/g;s/LINKINDYN.*file.plug/#&/g;}' < ./config.bak > ./config

windebug: realclean windebugconfig dep all

winbug:
	make windebug > /tmp/pderror 2>&1 3>&1
#s/configure.in/configure.ac/g
src/defines.h.in: configure.ac	
	echo '#ifndef DEFINES_H' >src/defines.h.in
	echo '#define DEFINES_H' >>src/defines.h.in
	sed -f mkdefs.sed < configure.ac|grep '^#undef'|sort|uniq>> src/defines.h.in
	echo '#endif' >>src/defines.h.in
show:
	echo $(PROGMANEXTRA)


left:
	find . -name *.o* -o -name *.i -o -name *.ii* -o -name *core* -o -name *.exe -o -name *.bak -o -name *~ 
	-ls pkg/work 
	-ls debian/tmp

linuxall: linuxconfig dep all 

DEBPACKAGE=$(DISTPATH)/download/bed_$(VERSION)_i386.deb
DEBPATCH=$(DISTPATH)/download/bed_$(VERSION)_i386.deb.patch

patch: 	$(DEBPACKAGE)
	binpatch $(FULLELFFILE) $(DEBPACKAGE) $(DEBPATCH) -c -l -O

deb: 	$(DEBPACKAGE)

debstart:
	cat debhead > $@
	./getsize $(FULLELFFILE) >>$@
$(DEBPACKAGE):debstart $(FULLELFFILE)
	cat debstart $(FULLELFFILE) >>$@

window:
	ls  -l  '$(ALLUSERSPROFILE)/Menu Start/bed.lnk'

dubwindist:
		make realclean
		make winconfig;make dep;make
		make dubrest

	
dubrest:
	make install
	make dubrestinstalled

dubrestinstalled:
		mv $(WINDIR)/bed.exe $(WINDIR)/bedp.exe
		make realclean
		make nwinconfig;make dep;make
		make install
		mv $(WINDIR)/bed.exe $(WINDIR)/bedn.exe
		make bedpatch
bedpatch:
		cp e:/usr/bin/binpatchg.exe 'c:/Program Files/bed/binpatch.exe'
		binpatch $(WINDIR)/bedp.exe $(WINDIR)/bedn.exe 'c:/Program Files/bed/bedpatch' -c -l -O -u5
		cp $(WINDIR)/bedp.exe $(WINDIR)/bed.exe
towin:
	cp $(DISTPATH)/download/$(PROGRAM)-$(VERSION).src$(TGZSUF) /c/cygwin/home/jka/html/download
	cp $(DISTPATH)/download/$(ELFARGFILE) /c/cygwin/home/jka/html/download

dochier: 
	$(MAKE) -C doc bed.html
	$(MAKE) -C doc bed.txt
	cp doc/bed.html bed.htm
	cp doc/bed.txt bed.txt
	u2d bed.txt

ifneq ($(HAS_RE_SEARCH),y)
$(REGEX):
		$(MAKE) CFLAGS="$(CFLAGS)" -C  $(REGEX_DIR) regex.o
endif

ls:
	-ls -l    $(ROOTDIR)$(BINDIR)/$(EXEPROGRAM)
	-ls -l    $(ROOTDIR)$(CONFDIRLINK)
	-ls -l   $(ROOTDIR)$(MENUPATH)/$(PROGRAM)
	-ls -l    $(CONFDIRLINK)
	-ls -l   $(BINDIR)/$(EXEPROGRAM)
	-ls -l   $(CONFDIR)


flags:
	echo $(CFLAGS)


os:
	echo $(OS)
ifeq ($(OpSys),Windows)
	echo window
else
	echo not	
endif

windir:
	echo $(WINDIR)
	echo $(windir)
zipcom:
	echo 	zip -z -r ../html/download/bed-$(VERSION).win32.zip bedwin32.txt  $(CONFDIR) $(BINDIR)/cygwin1.dll $(BINDIR)/$(EXEPROGRAM) '$(ALLUSERSPROFILE)/Menu Start/bed.lnk' 'c:/windows/Start Menu/bed.lnk'

GET=1
res=$(shell if test $(GET) -gt 2;then echo groter;else echo kleiner ;fi)
#res=$(shell echo hallo;echo niets)
gets:
	echo $(res)


copyright:
	ln  -s LICENSE $(BEDBASE).copyright

pad:
	bash ../html/mkbedpad.sh $(VERSION)


sign2: $(DISTPATH).sign2 
%.sign2:
	-$(PGPSIGNPROG)   --comment $(RPMFILE) $*/download/$(RPMFILE) 
	-$(PGPSIGNPROG)   --comment $(NONSTATICARGFILE32) $*/download/$(NONSTATICARGFILE32) 
	-$(PGPSIGNPROG)   --comment $(NONSTATICARGFILE64) $*/download/$(NONSTATICARGFILE64) 
	 $(PGPSIGNPROG)   --comment $(NONSTATICARGFILE_ARMHF) $*/download/$(NONSTATICARGFILE_ARMHF) 
	 $(PGPSIGNPROG)   --comment $(ANDROIDARGFILE) $*/download/$(ANDROIDARGFILE) 
	-$(PGPSIGNPROG)   --comment $(SRCARGFILE) $*/download/$(SRCARGFILE) 
	-$(PGPSIGNPROG)   --comment $(WIN32ARGFILE) $*/download/$(WIN32ARGFILE) 
	-$(PGPSIGNPROG)   --comment $(NWIN32ARGFILE) $*/download/$(NWIN32ARGFILE) 
	-$(PGPSIGNPROG)   --comment $(WIN95ARGFILE) $*/download/$(WIN95ARGFILE) 
	-$(PGPSIGNPROG)   --comment $(FREEBSDARGFILE) $*/download/$(FREEBSDARGFILE) 
	-$(PGPSIGNPROG)   --comment $(OPENSOLARISFILE) $*/download/$(OPENSOLARISFILE) 
	-make $*/download/$(LSMFILE) 


sign3: $(DISTPATH).sign3 
%.sign3:
	$(PGPSIGNPROG)   --comment $(SRCARGFILE) $*/download/$(SRCARGFILE) 
	-$(PGPSIGNPROG)   --comment $(WIN64ARGFILE) $*/download/$(WIN64ARGFILE) 
#	$(PGPSIGNPROG)   --comment $(NWIN64ARGFILE) $*/download/$(NWIN64ARGFILE) 
	$(PGPSIGNPROG)   --comment $(NONSTATICARGFILE64) $*/download/$(NONSTATICARGFILE64) 
	-$(PGPSIGNPROG)   --comment $(ANDROID64ARGFILE) $*/download/$(ANDROID64ARGFILE) 

#UNINSTALLFILE=$(ROOTDIR)$(CONFDIR)/uninstallbed.sh
UNINSTALLFILE=/tmp/uninstallbed.sh

#s/$(ROOTDIR)$(CONFDIR)\/uninstallbed.sh/$(UNINSTALLFILE)/g

mkun:
	@cp uninstallhead $(UNINSTALLFILE)
	@echo $(BINDIST)
	@FILES=`for afile in $(BINDIST); do  test -e $$afile -o -L $$afile&&echo $$afile ;done`;if test . != .$(ROOTDIR); then SLASHROOT=`echo $(ROOTDIR)| sed -e 's/\\//\\\\\\//g'`;fi;find  $$FILES ! -type d -print |if test . != .$(ROOTDIR);then sed -e "s/$$SLASHROOT//g";else cat;fi |xargs echo $(REMOVE) ' ' >> $(UNINSTALLFILE) ;find  $$FILES -type d  -a  -print| if test . != .$(ROOTDIR);then sed -e "s/$$SLASHROOT//g";else cat;fi |$(SORT) -r|xargs echo $(RMDIR) ' ' >> $(UNINSTALLFILE)
	@echo Uninstall with sh $(UNINSTALLFILE)


hosttest:hosttest.cc
	$(CXX) $(CXXFLAGS)  $< -o $@

hasf:
	echo $(HAS_FILE);
showvar:
	echo $(BINDIR)



hasreg:
	echo .$(HAS_RE_SEARCH).
ifneq ($(HAS_RE_SEARCH),y)
	echo no HAS_RE_SEARCH yes
endif
build2:
	grep  BUILDEXEEXT build > build2
	echo "HAS_FILE := $(shell $(BEDSRCDIR)/has_file)" >> build2

# sed -e '/.files. . ./r termux-files' < termux-manifest.in   > /tmp/uit2
termux-files.in: termux-root Makefile
	find termux-root/data/data/com.termux/files/usr/ > termux-files.in

termux-files: termux-files.in
	sed -e 's/^termux-root\/data\/data\/com.termux\/files\/usr\/\(.*\)$$/\"&\": \"\1\",/'  termux-files.in |head -c -2 > termux-files
#	sed -e 's/^.*$$/\"termux-root\/data\/data\/com.termux\/files\/usr\/&\": \"&\",/'  termux-files.in |head -c -2 > termux-files

termux-manifest.json:termux-manifest.in config termux-files
	sed -e '/files/r termux-files' -e "s/VERSION/$(VERSION)/" -e "s/HOST_CPU/$(HOST_CPU)/" -e "s/TITLE/`cat title`/" < termux-manifest.in > termux-manifest.json





wininstall: $(CONFFILE)  #$(LSMFILE)
	-$(MKDIR) $(ROOTDIR)$(BINDIR)
	-rm   $(ROOTDIR)$(BINDIR)/$(EXEPROGRAM)
	$(CP) src/$(EXEPROGRAM) $(ROOTDIR)$(BINDIR)
	$(STRIP)  $(ROOTDIR)$(BINDIR)/$(EXEPROGRAM)
	-rm   $(ROOTDIR)$(CONFDIRLINK)
	-mv -f  $(ROOTDIR)$(CONFDIRLINK) $(ROOTDIR)$(CONFDIRLINK).bak
	-$(MKDIR) $(ROOTDIR)$(CONFDIR)
	$(CP) $(CONFFILE) $(ROOTDIR)$(CONFDIR)
	grep  BUILDEXEEXT build > $(ROOTDIR)$(CONFDIR)/build
	echo "HAS_FILE := $(shell $(BEDSRCDIR)/has_file)" >> $(ROOTDIR)$(CONFDIR)/build
	chmod 0644 	$(ROOTDIR)$(CONFDIR)/$(CONFFILE)
	$(MAKE) -C doc install ROOTDIR=$(ROOTDIR)
	$(MAKE) -C misc install ROOTDIR=$(ROOTDIR)
ifeq ($(PDCURSES),)
	echo $(EXEPROGRAM) -k %1 %2 %3 %4 %5 %6 %7 %8 %9 > $(ROOTDIR)$(CONFDIR)/keyconfig.bat
	cp -a  terminfo $(ROOTDIR)$(CONFDIR)
endif
	cp    LICENSE $(ROOTDIR)$(CONFDIR)
ifeq ($(OpSys),Windows)
	u2d     $(ROOTDIR)$(CONFDIR)/LICENSE
endif
	cp doc/datadir.txt $(ROOTDIR)$(CONFDIR)/README
	cp public.pgp $(ROOTDIR)$(CONFDIR)
ifeq ($(HAS_FILE),y)
	$(CP) testmarks.bedm bedmagic bedmagic.short addmagic $(ROOTDIR)$(CONFDIR)
endif
	cp -a win/* $(ROOTDIR)$(prefix)
	@echo Binary: '$(ROOTDIR)$(BINDIR)/$(EXEPROGRAM)'
	@echo Support files: $(ROOTDIR)$(CONFDIR)

