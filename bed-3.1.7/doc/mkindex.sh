#!/bin/bash
positions=`grep -n '^<HR>$' bed.html.tus2 |cut -f 1 -d :`
declare -a pos
pos=($positions)
start=$[${pos[0]}+2]
end=${pos[1]}
len=$[$end-$start]
#cat menu.html.instart  trackingcode.txt menu.html.nahead > menu.html
cat menu.html.instart   menu.html.nahead > menu.html
tail  -n +${start} < bed.html.tus2 |head -n ${len}|sed -e '{s/HREF=\"#/TARGET=\"text\" HREF=\"bed.1.html#/g;s/<DD>//g;s/<DL>/<DL COMPACT>/g;}'|sed -e 's/^\(.*\)bed.1.html[^"]*\(">*DOWNLOAD.*\)$/\1download.html\2/' >> menu.html
cat menu.html.inend >> menu.html
