# Makefile for regex.
# 
# Copyright (C) 1992, 1993 Free Software Foundation, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

version = 0.13

# You can define CPPFLAGS on the command line.  Aside from system-specific
# flags, you can define:
#   -DREGEX_MALLOC to use malloc/realloc/free instead of alloca.
#   -DDEBUG to enable the compiled pattern disassembler and execution
#           tracing; code runs substantially slower.
#   -DEXTRACT_MACROS to use the macros EXTRACT_* (as opposed to
#           the corresponding C procedures).  If not -DDEBUG, the macros
#           are used.
CPPFLAGS = 

# Likewise, you can override CFLAGS to optimize, use -Wall, etc.
CFLAGS = -g

# Ditto for LDFLAGS and LOADLIBES.
LDFLAGS =
LOADLIBES =

srcdir = .


CC = gcc
DEFS = -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE_URL=\"\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -D__EXTENSIONS__=1 -D_ALL_SOURCE=1 -D_GNU_SOURCE=1 -D_POSIX_PTHREAD_SEMANTICS=1 -D_TANDEM_SOURCE=1 -DHAVE_GETMNTENT=1 -DSTDC_HEADERS=1 -DHAVE_STRING_H=1 -DHAVE_ALLOCA_H=1 -DHAVE_ALLOCA=1

SHELL = /bin/sh

subdirs = doc test

default all:: regex.o
.PHONY: default all


regex.o: regex.c xregex.h xregex2.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(DEFS) -I. -I$(srcdir) -c $<

configure: configure.in
	autoconf

config.status: configure
	sh configure --no-create

Makefile: Makefile.in config.status
	sh config.status

makeargs = $(MFLAGS) CPPFLAGS='$(CPPFLAGS)' CFLAGS='$(CFLAGS)' CC='$(CC)' \
DEFS='$(DEFS)' LDFLAGS='$(LDFLAGS)' LOADLIBES='$(LOADLIBES)'

default all install TAGS check::
	for d in $(subdirs); do (cd $$d; $(MAKE) $(makeargs) $@); done

# 11/15/06 jpr5: effort to force this makefile to clean up completely
#                re-organized the clean statements
#                added config.cache and config.log to targets                 

clean mostlyclean:
	for d in $(subdirs); do (cd $$d; $(MAKE) $(makeargs) $@); done
	rm -f *.o

extraclean realclean distclean:
	for d in $(subdirs); do (cd $$d; $(MAKE) $(makeargs) $@); done
	rm -f patch* *~* *\#* *.orig *.rej *.bak core a.out
	rm -f Makefile config.status config.cache config.log regex.o

.PHONY: install mostlyclean clean distclean extraclean realclean TAGS check


# Prevent GNU make 3 from overflowing arg limit on system V.
.NOEXPORT:

distfiles = AUTHORS ChangeLog COPYING INSTALL NEWS README \
            *.in configure regex.c regex.h 
distdir = regex-$(version)
distargs = version=$(version) distdir=../$(distdir)/$$d
dist: TAGS configure
	@echo "Version numbers in: Makefile.in, ChangeLog, NEWS,"
	@echo "  regex.c, regex.h,"
	@echo "  and doc/xregex.texi (if modified)."
	rm -rf $(distdir)
	mkdir $(distdir)
	ln $(distfiles) $(distdir)
	for d in $(subdirs); do (cd $$d; $(MAKE) $(distargs) dist); done
	tar czhf $(distdir).tar.Z $(distdir)
	rm -rf $(distdir)
.PHONY: dist
