include ../config
export LC_ALL=C
CFLAGS+=-O3 -s 
#PROGRAMS=getch mkkeydef  bin2byte
PROGRAMS=bin2byte 
EXEPROGRAMS= $(patsubst %,%$(EXEEXT),$(PROGRAMS))
EXEBYTEC= $(patsubst %,%$(EXEEXT),bin2bytec)

UTILSDIR=$(CONFDIR)/utils
LIBS= $(USENCURSESLIB)
LOADLIBES= $(USENCURSESLIB)

ifneq ($(CROSS_COMPILING),yes)
ifeq ($(TOBUILDCFLAGS),)
	TOBUILDCFLAGS+= -DONE
	TOBUILDLDFLAGS+= -DONE
endif
endif

ifeq ($(CROSS_COMPILING),yes)
all: $(PROGRAMS) bin2bytec
else
all: $(PROGRAMS)
endif
SRC :=  bin2byte.c features.h

bin2byte: bin2byte.c
	$(TOBUILDCC)  $(TOBUILDCFLAGS) $(TOBUILDLDFLAGS) $< -o $@ 
bin2bytec: bin2byte.c
	$(CC)  $(CFLAGS) $(PROGRAMC) $(LDFLAGS) $(PROGRAMLD) $< -o $@ 
mkkeydef: mkkeydef.c
	$(TOBUILDCC)  $(TOBUILDCFLAGS) $< -o $@ $(LIBS)
#keyconfig: keyconfig.c
#	$(TOHOSTCXX) $(CFLAGS) $< -o $@ $(LIBS)
getch.o: getch.c
	$(TOBUILDCXX) -c -DHAS_BOOL -D_GNU_SOURCE -DCURSES $(CFLAGS) $< -o $@
%.i:%.c;
	$(TOBUILDCC) $(TOBUILDCFLAGS) -E $< -o $@ 
realclean: clean
clean:
	-rm	$(EXEPROGRAMS) core $(EXEBYTEC)

install: $(PROGRAMS)
	-mkdir -p $(ROOTDIR)$(UTILSDIR)
	cp $(SRC) $(ROOTDIR)$(UTILSDIR)
ifeq ($(CROSS_COMPILING),yes)
ifneq ($(CYGWIN),yes)
	cp  bin2bytec $(ROOTDIR)$(UTILSDIR)/bin2byte
else
	cp  bin2bytec.exe $(ROOTDIR)$(UTILSDIR)/bin2byte.exe
endif
else
ifneq ($(CYGWIN),yes)
	cp  $(PROGRAMS) $(ROOTDIR)$(UTILSDIR)
else
	cp  bin2byte.exe $(ROOTDIR)$(UTILSDIR)
endif
endif
