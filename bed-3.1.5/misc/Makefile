include ../config
ifeq ($CONFDIR),)
	export MISCDIR=.
else
	export MISCDIR=$(CONFDIR)/misc
endif

GENERAL= README.txt filepermissionbits.bedda char2.bedda char2b.bedda hexasc.bedda pushlpushl.bedda
ELFTYPE= dynsym.bedda sectionheader.bedda symtab.bedda elfheader32.bedda elfheader64.bedda programheaderELF32.bedda programheaderELF64.bedda
BEDFORMATS=macroheader.bedda markheader.bedda
WIN32TYPE= win32sections.bedda #dosheader.bedda
LINUXTYPE=wtmp-linux.bedda rpm.bedda
FREEBSDTYPE=wtmp-freebsd.bedda
install: 
	-$(MKDIR) $(ROOTDIR)$(MISCDIR)
	$(CP) $(GENERAL) $(ROOTDIR)$(MISCDIR)
ifeq ($(USES_ELF),y)
	$(CP) $(ELFTYPE) $(ROOTDIR)$(MISCDIR)
endif
ifeq ($(SYSTEM),Linux)
	$(CP) $(LINUXTYPE) $(ROOTDIR)$(MISCDIR)
endif
ifeq ($(SYSTEM),FreeBSD)
	$(CP) $(FREEBSDTYPE) $(ROOTDIR)$(MISCDIR)
endif
	$(CP) $(WIN32TYPE) $(ROOTDIR)$(MISCDIR)
	

#	mkdir  $(MISCDIR)
