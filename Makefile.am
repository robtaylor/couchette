SUBDIRS = compat vendor/json-glib

bin_PROGRAMS = couchette btest

couchette_SOURCES = \
	main.c \
	btree.c \
	btree.h

AM_CFLAGS = -static -g -std=c99 -D_GNU_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
AM_CFLAGS += -I$(srcdir)/compat
AM_CFLAGS += -I$(srcdir)/vendor/json-glib
AM_CFLAGS += @GLIB_CFLAGS@ @LIBSOUP_CFLAGS@

couchette_LDADD = @GLIB_LIBS@ @LIBSOUP_LIBS@ $(top_builddir)/vendor/json-glib/json-glib/libjson-glib-1.0.la

btest_SOURCES = \
	btest.c \
	btree.c \
	btree.h
