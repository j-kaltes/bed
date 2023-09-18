s/prockey \(\<[^ 	]*\>\)/<A HREF="#\1">&<\/A>/g
s/proconly \(\<[^ 	]*\>\)/<A HREF="#\1">&<\/A>/g
s/^#endif/<xxx>&/g
s/^#\([^ 	]*\)$/&\
<\/center>/g
{s/Content-type: text\/html.*$//g;s/^.*Return to Main Contents.*$//g;s/^Section: User Commands.*$//g;s/<BODY>/<body text="#000000" bgcolor="#FFFFFF" link="#0000EF" vlink="#55188A" alink="#FF0000">/g;}
s/<H1>bed<\/H1>//
#s/<H1>bed<\/H1>/<DIV ALIGN=right> <a href="http:\/\/www.nedstat.nl\/cgi-bin\/viewstat?name=linuxbed000"><img src="http:\/\/www.nedstat.nl\/cgi-bin\/nedstat.gif?name=linuxbed000" border=0 alt="" width=32 height=32><\/a><\/DIV>/
s/<\/BODY>//g
s/<\/HTML>//g
