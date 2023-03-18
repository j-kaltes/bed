cat <<!
Source: bed
Section: utils
Priority: optional
Maintainer: Jaap Korthals Altes <binaryeditor@gmx.com>
Standards-Version: $VERSION

Package: bed
Architecture: any
!
#echo -n "Architecture: "
#dpkg-architecture -q DEB_HOST_ARCH
#amd64, i386, armel, armhf, 
#Depends: ${shlibs:Depends}, libncurses5, libmagic1, libre2-3, libhyperscan4
cat <<"!"
Depends: ${shlibs:Depends}
!
echo -n 'Description: '
cat < title
sed -e 's/^[ 	]*/ /g' < description
