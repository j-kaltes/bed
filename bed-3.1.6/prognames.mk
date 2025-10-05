ifneq ($(INCLUDEDPROGNAMES),yes)
INCLUDEDPROGNAMES=yes
export INSTALL=install -c
export CP=cp -f
REMOVE=rm 
RMDIR=rmdir 
SORT=sort
ifeq ($(GNUSTRIP),yes)
STRIP=$(STRIPPROG) -s -R .note -R .comment -R .note.ABI-tag
else
STRIP=$(STRIPPROG) 
endif
export MKDIR=mkdir -p
SYMLINK=$(LN_S)
endif
