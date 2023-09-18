TMPFILE=`mktemp /tmp/bederrorsXXXXXX`
echo Outputfile: $TMPFILE
BEDSRCDIR=`pwd`
make BEDSRCDIR=$BEDSRCDIR windebugconfig2  > $TMPFILE 2>&1 3>&1
make BEDSRCDIR=$BEDSRCDIR dep >> $TMPFILE 2>&1 3>&1
make BEDSRCDIR=$BEDSRCDIR >> $TMPFILE 2>&1 3>&1


