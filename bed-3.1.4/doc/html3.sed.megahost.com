s/^#\([^ 	]*\)$/<p><A NAME="\1"><b>\1<\/b><\/a><center>/g
s/prockey \(\<[^ 	]*\>\)/Alt-t,d,\1/g
s/XYHOSTYXQRPATHRQ.\([2-9][^+]*\).\([^"]*\)\"/www.megawebhost.com\/cgi-bin\/man2html?cgi_command=\2\&cgi_section=\1\"/g
s/XYHOSTYXQRPATHRQ.\(1[^+]*\).\([^"]*\)\"/www.megawebhost.com\/cgi-bin\/man2html\?cgi_command=\2\"/g
s/XYHOSTYXQRPATHRQ..man2html/www.ambienteto.arti.beniculturali.it\/cgi-bin\/man2html\"\>man2html/
