#!/bin/sh

LIB_BUILDDIR=/home/mastersan/src/c/libs/bitmapset/bitmapset/build
LIB_LDFLAGS=

export prefix=${LIB_BUILDDIR}/usr

LIB_CFLAGS="-g3 -O0"


#SILENT=-s
SILENT=

true \
&& echo "make static" \
&& \
    make ${SILENT} shared \
        CFLAGS="${LIB_CFLAGS}" \
        LDFLAGS="${LIB_LDFLAGS}" \
        BUILDDIR="${LIB_BUILDDIR}/" \
\
&& echo "make test" \
&& \
    make ${SILENT} test \
        CFLAGS="${LIB_CFLAGS}" \
        CXXFLAGS="${LIB_CFLAGS} -I/usr/include/catch/" \
        BUILDDIR="${LIB_BUILDDIR}/" \
