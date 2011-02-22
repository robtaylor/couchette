SUBDIRS = compat

bin_PROGRAMS = tinycouch

tinycouch_SOURCES = \
	main.c \
	btree.c \
	btree.h

AM_CFLAGS = -g -std=c99 -D_GNU_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
AM_CFLAGS += -I$(srcdir)/compat
AM_CFLAGS += @GLIB_CFLAGS@

tinycouch_LDADD = @GLIB_LIBS@

