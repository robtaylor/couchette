#! /bin/sh

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

PKG_NAME=TinyCouch
TEST_TYPE=-f
FILE=main.c

test ${TEST_TYPE} ${FILE} || {
        echo "You must run this script in the top-level ${PKG_NAME} directory"
        exit 1
}

which gnome-autogen.sh || {
        echo "*** You need to install gnome-common from GNOME Git:"
        echo "***   git clone git://git.gnome.org/gnome-common"
        exit 1
}

REQUIRED_AUTOMAKE_VERSION=1.11 USE_GNOME2_MACROS=1 . gnome-autogen.sh 

libtoolize -i

cd vendor/json-glib && ./autogen.sh
