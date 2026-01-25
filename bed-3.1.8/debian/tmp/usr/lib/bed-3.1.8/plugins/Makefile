export LC_ALL=C
include ../config
ifeq ($(CONFIG_LARGEFILES),y)
CFLAGS := $(CFLAGS) -DLARGEFILES
endif
ifeq ($CONFDIR),)
	export PLUGINDIR=.
else
	export PLUGINDIR=$(CONFDIR)/plugins
endif


GENERAL=$(PLUGINDIR)/plugin.cpp
HEADERS=$(PLUGINDIR)/faked.h $(PLUGINDIR)/mainprocedures.h $(PLUGINDIR)/screenpart.h $(PLUGINDIR)/offtypes.h  $(PLUGINDIR)/global.h $(PLUGINDIR)/typelist.h $(PLUGINDIR)/filter.h


#ALLFILES= Makefile config  faked.h filter.h global.h offtypes.h plugin.cpp procedures.h screenpart.h typelist.h
ALLFILES = Makefile faked.h global.h plugin.cpp 
FROMSRCDIR=ascii.h contain.h filter.h dialog.h hassub.h multidigit.h offtypes.h mainprocedures.h repeat.h screen.h screenpart.h type.h typelist.h views.h digitstring.h config.h varmacro.h  readconfig.h



CFLAGS := $(CFLAGS) -g



#%.plug: %.o
#	gcc $(CFLAGS) -shared $< -o $@
#	chmod a-x $@
#%.o: %.cpp  $(GENERAL) $(HEADERS)
#	gcc $(CFLAGS) -DSOURCEFILENAME=\"$<\" -c $(GENERAL) -o $@
all: mainprocedures.h

mainprocedures.h:../src/mainprocedures.h
	ln -fs ../src/mainprocedures.h $@
../src/mainprocedures.h:
	$(MAKE) -C ../src mainprocedures.h

install:
	-mkdir -p $(ROOTDIR)$(CONFDIR)
	-mkdir  -p $(ROOTDIR)$(PLUGINDIR)
	cp ../config $(ROOTDIR)$(CONFDIR)
	cp -f $(ALLFILES) $(ROOTDIR)$(PLUGINDIR)
	(CURDIR=`pwd`;cd $(ROOTDIR)$(PLUGINDIR) ;chmod  0644 $(ALLFILES);cd $$CURDIR)
	(cd ../src; $(CP) $(FROMSRCDIR) $(ROOTDIR)$(PLUGINDIR); $(CP) definesonly.h $(ROOTDIR)$(PLUGINDIR)/defines.h)

	$(MAKE) -C examples install

realclean:
	$(MAKE) -C examples realclean

clean:
	$(MAKE) -C examples clean

dep:
	$(MAKE) -C examples dep

