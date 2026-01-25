ELFFILE64=$1
cat <<!
If dpkg -i doesn't work, use following to unpack $ELFFILE64:

ar x $ELFFILE64 
tar -zxf control.tar.gz 
sh preinst
tar -zxf data.tar.gz -C /
sh postinst

bash /usr/share/bed/uninstallbed.sh 
will uninstall bed
!
