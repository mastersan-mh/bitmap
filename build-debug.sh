#!/bin/sh

LIB_BUILDDIR=./build-debug/
LIB_LDFLAGS=

export prefix=${LIB_BUILDDIR}/usr

LIB_CFLAGS="-g3 -O0"

#SILENT=-s
SILENT=

true \
&& echo "make clean-obj" \
&& make ${SILENT} clean-obj \
        BUILDDIR="${LIB_BUILDDIR}" \
        BUILDDIR_OBJ="${LIB_BUILDDIR}/obj-shared/" \
\
&& echo "make shared" \
&& make ${SILENT} shared \
        CFLAGS="${LIB_CFLAGS}" \
        LDFLAGS="${LIB_LDFLAGS}" \
        BUILDDIR="${LIB_BUILDDIR}" \
        BUILDDIR_OBJ="${LIB_BUILDDIR}/obj-shared/" \
\
&& echo "make clean-obj" \
&& make ${SILENT} clean-obj \
        BUILDDIR="${LIB_BUILDDIR}" \
        BUILDDIR_OBJ="${LIB_BUILDDIR}/obj-static/" \
\
&& echo "make test" \
&& make ${SILENT} test \
        CFLAGS="${LIB_CFLAGS}" \
        CXXFLAGS="${LIB_CFLAGS} -I/usr/include/catch/" \
        BUILDDIR="${LIB_BUILDDIR}" \
        BUILDDIR_OBJ="${LIB_BUILDDIR}/obj-static/" \
