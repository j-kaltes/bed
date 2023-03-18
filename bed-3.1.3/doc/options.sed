s/^[ 	]*\(-[^:]*\):\(.*\)$/.TP\
.B \1:\
\2/
s/^[^ 		][^:]*$/.PP\
&/
s/^Files:$/.PP\
&/
s/^Current configuration file.*$//g
s/^.*Press F1 in the program or read bed.1 for more information.*//
s/^.*No.*configuration.*file.*found.*$//g
s/^*$//g
s/^.*cygwin1.dll.*$//g
s/^[^-]*:.*$/.br\
&/
s/\\/\\\\/g
