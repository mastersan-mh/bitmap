#!/bin/sh

check_app()
{
    APP=$1
    STR=`whereis "${APP}" 2> /dev/null | sed "s/^${APP}://g"`
    test -z "${STR}"
    return $?
}

doxygen Doxyfile

if check_app dot ; then
    echo "[INFO]: 'dot' not found, so graphs not builded"
    echo "[INFO]: Install on Ubuntu: apt install graphviz"
fi
