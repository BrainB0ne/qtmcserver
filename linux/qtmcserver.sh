#!/bin/bash

if [ -L $0 ] ; then
    QTMCSDIR=$(dirname $(readlink -f $0)) ;
else
    QTMCSDIR=$(dirname $0) ;
fi

if [ -n "$LD_LIBRARY_PATH" ]; then
    export LD_LIBRARY_PATH="$QTMCSDIR/lib:$LD_LIBRARY_PATH"
else
    export LD_LIBRARY_PATH="$QTMCSDIR/lib"
fi

exec "$QTMCSDIR/qtmcserver"
