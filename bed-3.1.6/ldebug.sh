#!/bin/sh
TMPFILE=`mktemp /tmp/bederrorsXXXXXX`
echo Outputfile: $TMPFILE
make debug  > $TMPFILE 2>&1 3>&1
make dep >> $TMPFILE 2>&1 3>&1
make >> $TMPFILE 2>&1 3>&1


