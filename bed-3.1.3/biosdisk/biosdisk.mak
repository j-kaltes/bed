#
# Borland C++ IDE generated makefile
#
.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = Implib
BCC     = Bcc +BccW16.cfg 
TLINK   = TLink
TLIB    = TLib
BRC     = Brc
TASM    = Tasm
#
# IDE macros
#


#
# Options
#
IDE_LFLAGS =  -LG:\usr\BC45\LIB
IDE_RFLAGS = 
LLATW16_biosdiskdlib =  -f -Twd -c -C
RLATW16_biosdiskdlib =  -31
BLATW16_biosdiskdlib = 
CNIEAT_biosdiskdlib = -IG:\usr\BC45\INCLUDE -D
LNIEAT_biosdiskdlib = -x
LEAT_biosdiskdlib = $(LLATW16_biosdiskdlib)
REAT_biosdiskdlib = $(RLATW16_biosdiskdlib)
BEAT_biosdiskdlib = $(BLATW16_biosdiskdlib)

#
# Dependency List
#
Dep_biosdisk = \
   biosdisk.lib

biosdisk : BccW16.cfg $(Dep_biosdisk)
  echo MakeNode 

biosdisk.lib : biosdisk.dll
  $(IMPLIB) $@ biosdisk.dll


Dep_biosdiskddll = \
   biosdisk.obj\
   biosdisk.def

biosdisk.dll : $(Dep_biosdiskddll)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGS) $(LEAT_biosdiskdlib) $(LNIEAT_biosdiskdlib) +
G:\usr\BC45\LIB\c0dc.obj+
biosdisk.obj
$<,$*
G:\usr\BC45\LIB\import.lib+
G:\usr\BC45\LIB\mathwc.lib+
G:\usr\BC45\LIB\cwc.lib
biosdisk.def
|

biosdisk.obj :  biosdisk.c
  $(BCC)   -P- -c @&&|
 $(CEAT_biosdiskdlib) $(CNIEAT_biosdiskdlib) -o$@ biosdisk.c
|

# Compiler configuration file
BccW16.cfg : 
   Copy &&|
-R
-v
-vi
-H
-H=diskio2.csm
-3
-Fs-
-Ff
-Ff=1
-Vf
-O-c
-O-d
-H-
-H=
-mc
-WD
| $@


