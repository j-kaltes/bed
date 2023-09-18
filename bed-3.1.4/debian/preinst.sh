cat <<"!"
#!/bin/sh
!
tmpfile=`mktemp`
sed -e 's/[ 	\n]*//g' >$tmpfile <<! 
/usr/share/bed
/usr/local/share/bed 
$CONFDIRLINK 
!
filestmp=`sort  $tmpfile|uniq`
rm $tmpfile
files=`echo $filestmp`
cat <<!
for fil in $files
do
if test -f  \$fil/uninstallbed.sh
	then
wasdir=\`pwd\`
cd \$fil
LINKTO=\`readlink \$fil\`
if test ! -d \$LINKTO
then
LINKTO=\`dirname \$fil\`/\`readlink \$fil\`
fi
cd \$wasdir
echo previous version in \$LINKTO
if test $CONFDIRLINK = \$fil 
then
#	if test \$LINKTO = $CONFDIR
#	then
#		echo overwrite \$LINKTO
#	else
#		echo link to different dir \$LINKTO 
#	fi
	echo uninstall the old version with:
	echo bash \$LINKTO/uninstallbed.sh
	echo uninstall what is left of the new version with: 
	echo dpkg -r bed
	echo and reinstall bed
else
	echo press:
	echo bash \$LINKTO/uninstallbed.sh
	echo to uninstall previous version
	echo and hope that it doesn\\'t remove part of the new installation.
fi
fi
done
!
