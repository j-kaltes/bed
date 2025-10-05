cat <<!
<HTML><HEAD>
<TITLE>Index of /~bedlinux/download</TITLE>
!
#cat doc/trackingcode.txt
cat <<!
</HEAD>
<BODY>
<H1>Index of /~bedlinux/download</H1>
<PRE><IMG SRC="/icons/blank.gif" ALT="     "> Name                   Description
<HR>
<IMG SRC="/icons/back.gif" ALT="[DIR]"> <A HREF="/~bedlinux/">Parent Directory</A> 
!
#BEDVERSION=$UPVERSION.$MIDVERSION.$LOWVERSION
LOWER=${BEDVERSION/[0-9]*\.[0-9]*\./}
LIMIT=8
ITER=0
while test $ITER -le $LIMIT
do
cat <<!
<IMG SRC="/icons/unknown.gif" ALT="[   ]"> <A HREF="bed-$BEDVERSION.lsm">bed-$BEDVERSION.lsm</A>  
<IMG SRC="/icons/compressed.gif" ALT="[CMP]"> <A HREF="bed-$BEDVERSION.src.tar.gz">bed-$BEDVERSION.src.tar.gz</A> 
<IMG SRC="/icons/unknown.gif" ALT="[   ]"> <A HREF="bed-$BEDVERSION.src.tar.gz.asc">bed-$BEDVERSION.src.tar.gz.asc</A>
<IMG SRC="/icons/compressed.gif" ALT="[CMP]"> <A HREF="bed-$BEDVERSION.bin.ELF.tar.gz">bed-$BEDVERSION.bin.ELF.tar.gz</A>
<IMG SRC="/icons/unknown.gif" ALT="[   ]"> <A HREF="bed-$BEDVERSION.bin.ELF.tar.gz.asc">bed-$BEDVERSION.bin.ELF.tar.gz.asc</A>
<IMG SRC="/icons/compressed.gif" ALT="[CMP]"> <A HREF="bed-$BEDVERSION.FreeBSD.tar.gz">bed-$BEDVERSION.FreeBSD.tar.gz</A> 
<IMG SRC="/icons/unknown.gif" ALT="[   ]"> <A HREF="bed-$BEDVERSION.FreeBSD.tar.gz.asc">bed-$BEDVERSION.FreeBSD.tar.gz.asc</A>
<IMG SRC="/icons/compressed.gif" ALT="[CMP]"> <A HREF="bed-$BEDVERSION.win32.zip">bed-$BEDVERSION.win32.zip</A> 
<IMG SRC="/icons/unknown.gif" ALT="[   ]"> <A HREF="bed-$BEDVERSION.win32.zip.asc">bed-$BEDVERSION.win32.zip.asc</A>
<PP>

!
ITER=$[$ITER+1]
if test 1 -le $LOWER
then
	LOWER=$[$LOWER-1]
else
	break
fi
done

cat <<!
</PRE></BODY></HTML>

!
