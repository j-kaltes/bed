TMPFILE=`mktemp /tmp/bederrorsXXXXXX`
echo Outputfile: $TMPFILE
BEDSRCDIR=`pwd`
make BEDSRCDIR=$BEDSRCDIR nwinconfig > $TMPFILE 2>&1 3>&1
make dep >> $TMPFILE 2>&1 3>&1
make >> $TMPFILE 2>&1 3>&1


