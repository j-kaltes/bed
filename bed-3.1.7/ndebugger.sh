TMPFILE=`mktemp /tmp/ndebugerrorsXXXXXX`
echo Outputfile: $TMPFILE
if true
then
make realclean
echo 'CURSESLIB=ncurses' >> settings
bash ./configure
mv ./config ./config.bak
sed -e 's/^CONFIG_DEBUG=.*/CONFIG_DEBUG=y/g' < ./config.bak > ./config
make dep
make 
fi >  $TMPFILE 2>&1 3>&1
