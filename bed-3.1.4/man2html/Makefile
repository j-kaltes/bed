include /home/jka/src/bedall/bed-3.1.4/man2html/../build
CC = gcc
CFLAGS += -Wall -Wstrict-prototypes -Wmissing-prototypes
OBJECTS = man2html.o cgibase.o abbrev.o strdefs.o
EXEEXT = $(BUILDEXEEXT)
bindir = $(DESTDIR)$(PREFIX)/usr/bin
mandir = $(DESTDIR)$(PREFIX)${prefix}/share/man
vardir = $(DESTDIR)$(PREFIX)/var
httpdir = $(DESTDIR)$(PREFIX)/home/httpd
cgiowner = nobody
cgigroup = nobody

all: man2html$(EXEEXT) hman

man2html$(EXEEXT):	$(OBJECTS)
	$(CC) $(LDFLAGS) -o man2html$(EXEEXT) $(OBJECTS)

# man2html:	../src/version.h

# This installs the man2html utility
install:	man2html$(EXEEXT)
	mkdir -p $(bindir)
	install -m 755 man2html$(EXEEXT) $(bindir)
	mkdir -p $(mandir)/man1
	install -m 644 man2html.1 $(mandir)/man1/man2html.@man1ext@

install-scripts: install-man-scripts install-glimpse-stuff install-hman

# These are the scripts that allow pointing a browser at
#   http://localhost/cgi-bin/man/man2html
# to work.
install-man-scripts:
	mkdir -p $(httpdir)/cgi-bin/man
	mkdir -p $(httpdir)/cgi-aux/man
	install -m 755 scripts/cgi-bin/man/* $(httpdir)/cgi-bin/man
	install -m 644 scripts/cgi-aux/man/* $(httpdir)/cgi-aux/man
	install -d -o $(cgiowner) -g $(cgigroup) -m 775 $(vardir)/man2html
# (aux was renamed to cgi-aux since aux causes problems under DOS)

# If you have installed glimpse, and have compressed man pages,
# then perhaps you also want these filters.
install-glimpse-stuff:
	install -m 644 glimpse_filters $(vardir)/man2html/.glimpse_filters

# In order not to have to type a long command like
#   netscape http://localhost/cgi-bin/man/man2html?section+topic
# or
#   lynx lynxcgi:/home/httpd/cgi-bin/man/man2html?section+topic
# it is convenient to have some shell script as a wrapper.
# The script hman can be aliased to man. It uses an environment
# variable MANHTMLPAGER to find out which browser you use, and
# you can set MANHTMLHOST if the pages are not on localhost.
hman: hman.sh
	rm -f hman
	sed -e 's,%version%,@version@,' hman.sh > hman

install-hman: hman
	install -m 555 hman $(bindir)/hman
	install -m 644 hman.1 $(mandir)/man1/hman.@man1ext@

clean:
	rm -f core hman man2html$(EXEEXT) $(OBJECTS) *~

spotless:	clean
	rm -f Makefile

$(OBJECTS): defs.h
