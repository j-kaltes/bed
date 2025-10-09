#!/bin/sh
function findname {
	TRY="bed bash gzip tar rxvt"
	for PACKAGE in $TRY
	do
		if rpm -q $PACKAGE
		then
			return	;
		fi
	done
	for PACKAGE in $TRY
	do
		if dpkg -l $PACKAGE|grep Install
		then
			return	;
		fi
	done
}
function menuitem {
cat <<!
?package($PACKAGE): needs="$2" section="$1" title="$4" command="$3" longtitle="Adjustable dataformat binary editor" icon-="binary.xpm"
!
}
findname > /dev/null
#menuitem $XBEDMENU x11 "rxvt -e $BINDIR/$PROGRAM" Bed
#menuitem $XBEDMENU x11 "TERMS='rxvt xterm nxterm gnome-terminal konsole'; for i in \$TERMS; do if which \$i ; then  \$i  -e $BINDIR/$PROGRAM;exit ;fi done" BED
menuitem $BEDMENU text "env TERMINFO=$CONFDIRLINK/terminfo TERM=rxvt $BINDIR/$PROGRAM" "$MENUNAME"
menuitem $XBEDMENU x11 "rxvt +sb -e env TERMINFO=$CONFDIRLINK/terminfo TERM=rxvt $BINDIR/$PROGRAM" "$XMENUNAME"

