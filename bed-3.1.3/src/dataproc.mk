
#include ../config
LINKALL=$(LINKIN) 
dataproc.h: ../config dataproc.mk
	echo '/*WARNING generated based on ../config dataproc.mk, dont edit*/'  > dataproc.h
ifeq  ($(findstring signed.plug,$(LINKALL)),signed.plug)
	echo 'dataproc("~Signed",Signed)'>> dataproc.h
endif
ifeq  ($(findstring float.plug,$(LINKALL)),float.plug)
	echo 'dataproc("Fl~oat",Float)'>> dataproc.h
endif
ifeq  ($(findstring bitflags.plug,$(LINKALL)),bitflags.plug)
	echo 'dataproc("~BitFlags",BitFlags)'>> dataproc.h
endif

ifeq  ($(findstring bitcontains.plug,$(LINKALL)),bitcontains.plug)
#	echo 'menuproc("Bitfi~eld",bitfield)'>> dataproc.h
#	echo 'menuproc("~Change Bit",changebitfield)'>> dataproc.h
	echo 'menuproc("Bitfi~eld",bitfielder)'>> dataproc.h
endif
ifeq  ($(findstring translate.plug,$(LINKALL)),translate.plug)
	echo 'menuproc("Trans~late",translate)'>> dataproc.h
#	echo 'menuproc("C~hange Tra",changetranslate)'>> dataproc.h
#	echo 'menuproc("View",selectlabels)'>> dataproc.h
endif
	echo >> dataproc.h

#vnrfiatlhsobec



dataprocer.h: dataproc.h dataproc.mk
	echo ' /*WARNING generated based on ../config dataproc.mk, dont edit*/ ' > dataprocer.h
	echo -n "#define EXTRAPROC " >> dataprocer.h
	echo `cat dataproc.h` >> dataprocer.h
	echo -n '#define EXTRAFILTERS ' >> dataprocer.h
ifeq  ($(findstring duplicate.plug,$(LINKALL)),duplicate.plug)
	echo -n 'menuproc("Dupl~icate",duplicate) '>> dataprocer.h
endif
ifeq  ($(findstring skip.plug,$(LINKALL)),skip.plug)
	echo -n 'menuproc("S~kip",addskip)'>> dataprocer.h
endif
	echo -ne '\n#define EXTRAMOVE ' >> dataprocer.h
ifeq  ($(findstring nextitem.plug,$(LINKALL)),nextitem.plug)
	echo -n 'menuprock("Ne~xt Item","C-x",nextitem) ' >> dataprocer.h
	echo -n 'menuprock("B~ack Item","C-b",backitem)' >> dataprocer.h
	echo -n 'LINE,' >> dataprocer.h

endif
	echo >> dataprocer.h
	echo -ne '\n#define  CLIPBOARD ' >> dataprocer.h
ifeq  ($(findstring clipboard.plug,$(LINKALL)),clipboard.plug)
	echo -n 'menuproc("To C~lipboard",toclipboard) ' >> dataprocer.h
	echo -n 'menuproc("~From Clipboard",pastclipboard)' >> dataprocer.h
endif
	echo >> dataprocer.h

extrakeys.h: ../config dataproc.mk
	echo > extrakeys.h
ifeq  ($(findstring nextitem.plug,$(LINKALL)),nextitem.plug)
	echo ' /* WARNING generated based on ../config dataproc.mk, dont edit*/ ' > extrakeys.h
	echo -e "keydef(nextsubitem,24)\nkeydef(backsubitem,2)\n" >> extrakeys.h
endif
	echo >> extrakeys.h

