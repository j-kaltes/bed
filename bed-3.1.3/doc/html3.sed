s/^#\([^ 	]*\)$/<p><A NAME="\1"><b>\1<\/b><\/a><center>/g
s/prockey \(\<[^ 	]*\>\)/Alt-t,d,\1/g
s/proconly \(\<[^ 	]*\>\)/\1/g
#s/XYHOSTYXQRPATHRQ.\([1-9][^+]*\).\([^"]*\)\"/swoolley.org\/man.cgi\/\1\/\2\"/g
#s/XYHOSTYXQRPATHRQ.\([1-9][^+]*\).\([^"]*\)\"/linux.die.net\/man\/\1\/\2\"/g
s/XYHOSTYXQRPATHRQ.\([1-9][^+]*\).\([^"]*\)\"/www.tin.org\/bin\/man.cgi?section=\1\&topic=\2\"/g
s/XYHOSTYXQRPATHRQ..man2html/gittup.org\/cgi-bin\/man\/man2html"\>man2html/
