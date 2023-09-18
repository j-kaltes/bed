cat <<"!"
#!/bin/sh
!
cat <<!
ln -T -fs $CONFDIR $CONFDIRLINK
cd $CONFDIR
!
cat <<"!"
bash ./addmagic
if which update-menus ; then update-menus ; fi
!
