TMPFILE=`mktemp /tmp/ndebugerrorsXXXXXX`
echo Outputfile: $TMPFILE
make nwindebugconfig  > $TMPFILE 2>&1 3>&1
make dep >> $TMPFILE 2>&1 3>&1
make >> $TMPFILE 2>&1 3>&1


