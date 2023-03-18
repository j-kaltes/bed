 DATE=`date +"%d%b%y"`

 SRCFILE=$SRCBASE.tar.gz
 SRCLSM=$ELFBASE.lsm

 SRCSIZE=`ls -s --human-readable $DISTPATH/$SRCFILE|cut -d ' ' -f 1`

#ELFFILE=bed-$VERSION-1.i586.rpm
ELFFILE=$ELFBASE.tar.gz
 ELFSIZE=`ls -s --human-readable $DISTPATH/$ELFFILE|cut -d ' ' -f 1`
# ELFLSM=$ELFBASE.lsm
# bed-$VERSION.ELF.tar.gz
FREEBSDFILE=bed-$VERSION.FreeBSD.tar.gz
FREEBSDSIZE=`ls -s --human-readable $DISTPATH/$FREEBSDFILE|cut -d ' ' -f 1`


STATICLINUXFILE=bed-$VERSION.bin.ELF-static.tar.gz
#STATICLINUXSIZE=`ls -s --human-readable $DISTPATH/$STATICLINUXFILE|cut -d ' ' -f 1`


WIN32FILE=bed-$VERSION.win32.zip
WIN32SIZE=`ls -s --human-readable $DISTPATH/$WIN32FILE|cut -d ' ' -f 1`

WIN32NCURFILE=bed-$VERSION.win32ncurses.zip
WIN32NCURSIZE=`ls -s --human-readable $DISTPATH/$WIN32NCURFILE|cut -d ' ' -f 1`
TERMIDENT=`sed -n -f terref.sed doc/menu.html`

cat <<!
<HTML>
<HEAD>
   <META HTTP-EQUIV="Content-Type" CONTENT="text/html; charset=iso-8859-1">
   <META NAME="GENERATOR" CONTENT="Mozilla/4.05 [en] (X11; I; Linux 2.0.33 i586) [Netscape]">
   <META NAME="Author" CONTENT="Jaap Korthals Altes">
   <META NAME="Description" CONTENT="Description of bed (binary editor for linux). Urls to download the source and the Elf binary.">
   <META NAME="Keywords" CONTENT="Hex, editor, bed, binary, octal, decimal, ascii. Linux.">
<title>bed - download</title>
</HEAD>
<body text="#000000" bgcolor="#FFFFFF" link="#0000EF" vlink="#55188A" alink="#FF0000">
<div align=right><font face="Arial,Helvetica"><a href="bed.1.html">Manual</a></font></div>
<h1>download</h1>
<FONT SIZE=+1>Bed <B>source</B>:</FONT>
<br><font size=+1>
<A HREF="ftp://sunsite.unc.edu/pub/Linux/apps/editors/terminal/$SRCFILE">$SRCFILE (sunsite)</A>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
($SRCSIZE)&nbsp; ( <A HREF="ftp://sunsite.unc.edu/pub/Linux/apps/editors/terminal/$LSMFILE">LSM</A>
)
<A HREF="ftp://sunsite.unc.edu/pub/Linux/apps/editors/terminal/$SRCFILE.asc">SIGN</A>
</font>
<br><font size=+1>
<A HREF="download/$SRCFILE">$SRCFILE</A>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
($SRCSIZE)&nbsp; ( <A HREF="download/$LSMFILE">LSM</A>
)
<A HREF="download/$SRCFILE.asc">SIGN</A>
</FONT>

<P><FONT SIZE=+1><B>Linux</B> i386 binary:</FONT>	
<br><FONT SIZE=+1>
See <A HREF="bed.1.html#$TERMIDENT">Terminal emulation requirements</A>
<br>
<A HREF="ftp://sunsite.unc.edu/pub/Linux/apps/editors/terminal/$ELFFILE">$ELFFILE (sunsite)</A>&nbsp;&nbsp;&nbsp;&nbsp;
($ELFSIZE) (<A HREF="ftp://sunsite.unc.edu/pub/Linux/apps/editors/terminal/$LSMFILE">LSM</A>)
<A HREF="ftp://sunsite.unc.edu/pub/Linux/apps/editors/terminal/$ELFFILE.asc">SIGN</A>
</FONT>
<br><font size=+1>
<A HREF="download/$ELFFILE">$ELFFILE</A>&nbsp;&nbsp;&nbsp;&nbsp;
($ELFSIZE) ( <A HREF="download/$LSMFILE">LSM</>
) <A HREF="download/$ELFFILE.asc">SIGN</A>
</FONT>

<P><FONT SIZE=+1><B>FreeBSD</B> 4.4 binary:</FONT>
<br><font size=+1>
<A HREF="download/bed-0.2.22.FreeBSD.tar.gz">bed-0.2.22.FreeBSD.tar.gz</A>&nbsp;&nbsp;&nbsp;&nbsp;
(480K) <A HREF="download/bed-0.2.22.FreeBSD.tar.gz.asc">SIGN</A></FONT>
<!--
<A HREF="download/$FREEBSDFILE">$FREEBSDFILE</A>&nbsp;&nbsp;&nbsp;&nbsp;
($FREEBSDSIZE) <A HREF="download/$FREEBSDFILE.asc">SIGN</A></FONT>
// -->

<p><font size=+1><B>Windows95/Vista</B> cygwin binary (pdcurses):</font>
<br><font size=+1>
<a href="download/$WIN32FILE">$WIN32FILE</a>&nbsp;&nbsp;&nbsp;&nbsp;
($WIN32SIZE)
<a href="download/$WIN32FILE.asc">SIGN</a>
</font>
<br><font size=+1>
cygwin1.dll is now included in the zip file (source <a href="download/cygwin-1.7.5-1-src.tar.bz2">cygwin-1.7.5-1-src.tar.bz2</a>). The cygwin1.dll used by bed-0.2.25 is found by the <a href="/download">old versions of bed</a>. If you don't already have it, put it somewhere in your
path. Using multiple copies gives problems. Using a cygwin1.dll version very different from 1.7.5-1 also is likely to give problems.
Cygwin doesn't seems to have any solutions for this, so I should have used <a href="http://nuwen.net/mingw.html" >MinGW</a> instead.
If you want to run the MS Windows version of bed within xterm or rxvt, download also the following package and put bedn.exe in c:\Program Files\bed
<br>
<a href="download/$WIN32NCURFILE">$WIN32NCURFILE</a>&nbsp;&nbsp;&nbsp;&nbsp;
($WIN32NCURSIZE)
<a href="download/$WIN32NCURFILE.asc">SIGN</a>
<br>You can also replace the pdcurses version with this version: rename bedn.exe to bed.exe and put it in c:\windows
<br>Except for the need to download ncurses from <a href="http://cygwin.com" TARGET="_top">cygwin</a> and have a basal cygwin installation, I didn't notice the shortcomings 
of the previous ncurses versions when running bed in cmd.com
<p><font size=+1>To compile bed for win32 you need:
<ul><li>gcc, binutils, gnu make, grep, sed, textutils, findutils, fileutils, diffutils, which and probably some other utilities from <a href="http://sources.redhat.com/cygwin" TARGET="_top">cygwin</a>
<li>GNU regex (different from libregex.a from cygwin): e.g.&nbsp; <a href="http://ftp.kiarchive.ru/pub/unix/fileutl/regex-0.12.tar.gz" TARGET="_top">http://ftp.kiarchive.ru/pub/unix/fileutl/regex-0.12.tar.gz</a>
<li>pdcurses source (<a href="http://pdcurses.sourceforge.net" TARGET="_top">http://pdcurses.sourceforge.net</a>)
</ul>
<p>
<font size="+1"><A NAME="oldbeds">Old versions:</a> <a href="http://bedlinux.tripod.com/download">tripod</a> and <a href="http://bed.dse.nl/download">dse</a><br>
</font></p>
<p><font size="+1"><a href="pgpkey.bed">pgp public key</a>
</font></p>
<!-- Begin Nedstat Basic code -->
<!-- Title: bed - download -->
<!-- URL: http://bed.dse.nl/download.html -->
<script language="JavaScript" type="text/javascript" src="http://m1.nedstatbasic.net/basic.js">
</script>
<script language="JavaScript" type="text/javascript" >
<!--
  nedstatbasic("ACwMPArv2Wq+2bqtRRq3RpXdR7bA", 0);
// -->
</script>
<noscript>
<a target="_blank" href="http://www.nedstatbasic.net/stats?ACwMPArv2Wq+2bqtRRq3RpXdR7bA"><img
src="http://m1.nedstatbasic.net/n?id=ACwMPArv2Wq+2bqtRRq3RpXdR7bA"
border="0" width="18" height="18"
alt="Nedstat Basic - Gratis web site statistieken
Eigen homepage website teller"></a><br>
<a target="_blank" href="http://www.nedstatbasic.net/">Gratis teller</a>
</noscript>
<!-- End Nedstat Basic code -->



</body>
</html>
!
