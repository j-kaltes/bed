TMPFILE=`mktemp /tmp/bioserrorsXXXXXX`
echo Outputfile: $TMPFILE
if true
then
make realclean
echo 'CURSESLIB=pdcurses' >> settings
bash ./configure
mv ./config ./config.bak
sed -e 's/^CONFIG_DEBUG=.*/CONFIG_DEBUG=m/;s/^#\(export BIOSDISK=y\)$/\1/' < ./config.bak > ./config
make dep
make 
fi >  $TMPFILE 2>&1 3>&1
