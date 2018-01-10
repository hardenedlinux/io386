
OPT=-O2
CFLAGS=$(OPT) -Wall -W -Wno-parentheses -Wstrict-prototypes -finline-functions

# Host OS and release (override if you are cross-compiling)
HOST=
RELEASE=
CROSS_COMPILE=

# Installation directories
PREFIX=/usr/local
SBINDIR=$(PREFIX)/sbin
SHAREDIR=$(PREFIX)/share
IDSDIR=$(SHAREDIR)
MANDIR:=$(shell if [ -d $(PREFIX)/share/man ] ; then echo $(PREFIX)/share/man ; else echo $(PREFIX)/man ; fi)
INCDIR=$(PREFIX)/include
LIBDIR=$(PREFIX)/lib
PKGCFDIR=$(LIBDIR)/pkgconfig

# Commands
INSTALL=install
DIRINSTALL=install -d
STRIP=-s
CC=$(CROSS_COMPILE)gcc
AR=$(CROSS_COMPILE)ar
RANLIB=$(CROSS_COMPILE)ranlib

all: io386

io386: io386.c io386.h
	$(CC) $(CFLAGS) -o $@ $(LDFLAGS) $(TARGET_ARCH) $<

clean:
	rm io386

distclean: clean

install: all
# -c is ignored on Linux, but required on FreeBSD
	$(DIRINSTALL) -m 755 $(DESTDIR)$(SBINDIR)
	$(INSTALL) -c -m 755 $(STRIP) io386

uninstall: all
	rm -f $(DESTDIR)$(SBINDIR)/io386

.PHONY: all clean distclean install uninstall
