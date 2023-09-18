export LC_ALL=C
include ../config
ifeq ($(CONFIG_XBED),y)
$(XPROGRAM)one: ../src/rxvtmenu startbed afterrxvt endbed middlebed ../config Makefile
	cat startbed > $@
	echo -n $(RXVT) >> $@
	cat afterrxvt >> $@
	echo  exec $(BINDIR)/$(PROGRAM) '$$*'>>$@
	cat middlebed >> $@
	../src/rxvtmenu >> $@
	cat terminal >> $@
	cat endbed >> $@
	chmod a+x $@

$(XPROGRAM): bedtryhead middlebed endbed ../src/rxvtmenu  ../config Makefile
	echo '#!/bin/sh ' > $@
	echo RXVTS=\"$(RXVT) $(NRXVT)\" >>$@
	cat bedtryhead >> $@
	echo USERXVT=yes exec  $(BINDIR)/$(PROGRAM) '$$*' >>$@
	echo 'fi' >> $@
	cat middlebed >> $@
	../src/rxvtmenu >> $@
	cat terminal >> $@
	cat endbed >> $@
	chmod a+x $@

../src/rxvtmenu:../src/rxvtmenu.c
	$(MAKE) -C ../src rxvtmenu

install:
	-mkdir -p $(ROOTDIR)$(BINDIR)
	cp $(XPROGRAM)  $(ROOTDIR)$(BINDIR)/$(XPROGRAM)
else
install:
	echo rxvt with menu support needed for  $(ROOTDIR)$(XPROGRAM)
endif

clean:
	-rm xbed

realclean: clean
