SUBDIRS = compat

AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = tinycouch

tinycouch_SOURCES = \
	btree.c \
	btree.h

AM_CFLAGS = -g -std=c99 -D_GNU_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
AM_CFLAGS += -I$(srcdir)/compat

tinycouch_LDADD = @GLIB_CFLAGS@

