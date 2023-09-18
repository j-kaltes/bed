include ../config
# I saw no problems with -O2 now I removed some inline statements
# but I did most of my test without optimalisation. And the first
# release gave unexpected problems optimalised.
CFLAGS+= -Wno-unused-variable
ifeq ($(CONFIG_DEBUG),n)
	LDFLAGS+= -s
	CFLAGS+=  $(OPTS)  
else
	ifeq ($(CONFIG_DEBUG),o)
		LDFLAGS+= -O2 -g
		CFLAGS+=  $(OPTS) -g -O2
	else
		CFLAGS+=  $(OPTS) -g
		LDFLAGS+= -g
	endif
endif
all: biosdisk32.dll biosdisk.dll
biosdisk32.o: biosdisk32.c biosdisk.h biosthun.h 
	gcc  $(CFLAGS) -c biosdisk32.c 
biosdisk32.s: biosdisk32.c biosdisk.h biosthun.h 
	gcc -S $(CFLAGS) biosdisk32.c -o biosdisk32.s  
rmbioslib: rmbioslib.c biosdisk32.dll
	gcc -O3 -s rmbioslib.c biosdisk32.dll -o rmbioslib
biosdisk32.i: biosdisk32.c biosdisk.h biosthun.h 
	gcc $(CFLAGS) -E biosdisk32.c  -o biosdisk32.i
biosdisk.dll: biosdisk.c biosdisk.h biosthun.h biosdisk.def
	maker  -f biosdisk.mak 
	tdstrip biosdisk.dll


biosdisk32.dll: biosdisk32.o biosdisk32.def 
	gcc  -shared --dll $(LDFLAGS) biosdisk32.def biosdisk32.o  -o biosdisk32.dll 

install: biosdisk32.dll biosdisk.dll
	-mkdir -p $(ROOTDIR)$(CONFDIR)
	-cp biosdisk32.dll biosdisk.dll $(ROOTDIR)$(CONFDIR)

clean:
	-rm biosdisk32.dll biosdisk32.o biosdisk.obj biosdisk32.lib  rmbioslib.obj
realclean: clean
	-rm  biosdisk.dsw rmbioslib.exe

echo:
	echo $(CFLAGS)
	echo $(OPT)
	echo $(LDFLAGS)
