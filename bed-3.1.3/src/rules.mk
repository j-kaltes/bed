CALGFLAGS+=-fno-exceptions -fno-rtti

ifeq ($(CONFIG_STATIC),y)
CONFIG_PLUGIN=n
CALGFLAGS+=-DSTATIC=1
LDFLAGS := $(LDFLAGS) -static
endif

ifeq ($(CONFIG_THREADS),y)
CALGFLAGS := $(CALGFLAGS) -DUSETHREADS=1  -D_REENTRANT=1 -DLONGTHREADS
LIBS :=  $(THREADSLIB) $(LIBS)
endif

ifeq ($(CONFIG_PLUGIN),n)
CALGFLAGS := $(CALGFLAGS) -DNODL=1
else
RDYNAMIC := -rdynamic
LIBS := $(LIBS) $(DL_LIBRARY)
endif
LDFLAGS+=-fno-exceptions -fno-rtti

OPTIM=-O3
ifeq ($(CONFIG_DEBUG),y)
OPTS:= -gdwarf-4 -g3 $(OPTS)
CALGFLAGS :=  $(CALGFLAGS) -DTRACEON  -DDEBUGON -DDEBUGFLAG
OBJ :=  $(OBJ) debug.o myalloc.o 
else
	ifeq ($(CONFIG_DEBUG),n)
	OPTS := $(OPTIM) $(OPTS)
	CALGFLAGS+=-DNDEBUG=1
	#OPTS := $(OPTS)
	else
		ifeq ($(CONFIG_DEBUG),t)
			OPTS:=$(OPTIM) $(OPTS)
			CALGFLAGS :=  $(CALGFLAGS) -DTRACEON  -DDEBUGON -DDEBUGFLAG
			OBJ :=  $(OBJ) debug.o myalloc.o
		else	
			ifeq ($(CONFIG_DEBUG),o)
				OPTS:= -gdwarf-4 -g3 -O2 $(OPTS)
				CALGFLAGS :=  $(CALGFLAGS) -DDEBUGFLAG
			else
				ifeq ($(CONFIG_DEBUG),k)
					OPTS:= -gdwarf-4 -g3 -O       $(OPTS)
					CALGFLAGS :=  $(CALGFLAGS) -DDEBUGFLAG
				else
					OPTS:= -gdwarf-4 -g3 $(OPTS)
					CALGFLAGS :=  $(CALGFLAGS) -DDEBUGFLAG
				endif
			endif
		endif
	endif
endif

ifeq ($(CONFIG_MMAP),y)
CALGFLAGS := $(CALGFLAGS) -DUSE_MMAP=1
endif
ifeq ($(CONFIG_LARGEFILES),y)
CALGFLAGS := $(CALGFLAGS) -DLARGEFILES=8
endif

ifneq ($(LINKIN) $(LINKINDYN),)
ifneq ($(CYGWIN),yes)
ifneq ($(filter switchsize.plug putselect.plug, $(LINKIN) $(LINKINDYN)),)
#LIBS+=$(X_PRE_LIBS) -lX11 $(X_EXTRA_LIBS)
LIBS+= -lX11 $(X_EXTRA_LIBS)
CALGFLAGS+=$(X_FLAGS)
LDFLAGS+=$(X_LIBS)
endif
endif
INLINK= $(INLINKLIB)
INLINK_H= inlink.h
ifneq ($(LINKINDYN),)
CALGFLAGS+=-DFAKE_DYNAMIC=1
endif
else
INLINK=
INLINK_H=
endif

ifeq ($(CONFIG_WRITEBETWEENPARTS),y)
CALGFLAGS+= -DNOBACKGROUND  -DSPACEBETWEEN 
else
	ifeq ($(CONFIG_WRITEBETWEENPARTS),b)
	CALGFLAGS+= -DNOBACKGROUND  -DSPACEBETWEEN -DLINESBETWEEN
	else
		ifeq ($(CONFIG_WRITEBETWEENPARTS),c)
			CALGFLAGS+= -DNOBACKGROUND  -DSPACEBETWEEN -DLINESBETWEEN -DLINEAFTER
		else
			ifeq ($(CONFIG_WRITEBETWEENPARTS),i)
			CALGFLAGS+= -DNOBACKGROUND  -DSPACEBETWEEN -DLINESBETWEEN -DLINESBETWEENITEMS
			endif
		endif
	endif
endif
ifeq ($(CONFIG_MENULINES),y)
CALGFLAGS+= -DLINEMENUS
endif

ifneq ($(CONFIG_COLOR),n)
CALGFLAGS+=-DCOMPILECOLORS=1
	ifeq ($(CONFIG_COLOR),o)
	CALGFLAGS+=-DSTARTBLACKWHITE=1
	else
	CALGFLAGS+=-DSTARTBLACKWHITE=0
	endif
endif

#ifneq ($(CYGWIN),yes)
#OBJECTS+=win32.o
#endif
ifeq ($(CONFIG_XBED),y)
	CALGFLAGS+=-DUSE_WITH_XBED=1
endif
CFLAGS+=$(CALGFLAGS)
CXXFLAGS+=$(CALGFLAGS)

ifeq ($(HOST_OS),mingw32)
	CFLAGS+=-D_POSIX_THREAD_SAFE_FUNCTIONS=1
	CXXFLAGS+=-D_POSIX_THREAD_SAFE_FUNCTIONS=1
endif

