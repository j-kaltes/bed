TMPFILE=`mktemp /tmp/bederrorsXXXXXX`
echo Outputfile: $TMPFILE
echo 'CURSESLIB=pdcurses' >> settings
bash ./configure > $TMPFILE 2>&1 3>&1
mv ./config ./config.bak
sed -e '{s/^CONFIG_DEBUG=.*/CONFIG_DEBUG=y/g;s/LINKINDYN.*file.plug/#&/g;}' < ./config.bak > ./config
make dep >> $TMPFILE 2>&1 3>&1
make >> $TMPFILE 2>&1 3>&1


