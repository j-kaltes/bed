This directory contains saved composed datatypes. Pressing Alt-y,d
(Read Type) and open the datatype file to create such a datatype in
bed.

char2.bedda:   Show only even characters 
char2b.bedda:  Show only odd characters
filepermissionbits.bedda:    interpret date as file permissions (e.g. -r-xr-x )


hexasc.bedda: shows for every byte of the file a hexidecimal
followed by an ASCII.
hex/1:         68 65 6C 6C 6F 00 
ascii:         hello. 
hexascii:      68 h 65 e 6C l 6C l 6F o 00 .
     
wtmp-freebsd.bedda and wtmp-linux.bedda: formatted display of
wtmp like the program last does.

elfheader.bedda: 
displays the elfheader of a binary. It should be placed at offset 0 of
a binary and only the first item has any meaning. It is like the
output of readelf -h, but in bed you can easily edit it.

programheader.bedda:
Displays program header. It should be placed at offset 52 of the
binary. It is something like readelf -l

sectionheader.bedda:
Something like readelf -S. Jumps to the position of the section
headers in a elf executable and displays the section headers.

symtab.bedda
Displays .symtab symbol table. Jumps to appropriate offset or stays
at current position if this file has no .symtab symbol table. The same
data is shown by readelf -s

dynsym.bedda
Displays .dynsym symbol table. Jumps to appropriate offset or stays
at current position if this file has no .dynsym symbol table. 
The same data is shown by readelf -s

sectionheader.bedda, symtab.bedda and dynsym.bedda rely on the
plugins elftables.plug and index2table.plug.

win32sections.bedda: section table of win32 binaries. Search
	for .text or so to find out at what offset it should be placed.

rpm.bedda
Two datatypes: 
1. Displays start of rpm file;
2. Display rpm entries.


pushlpushl.bedda:   
Rather useless datatype that can find a long long in a binary in the
following particular case. Sometimes a long long digit in a program
is used by using two times a pushl with a constant digit. E.g.:
     pushl $776102145
     pushl $-2092083883
to push 3333333333333333333 on the stack. With this datatype you
can show the machine code (of pushl $776102145 pushl
$-2092083883 as 3333333333333333333. You can than regex search for
3333333333333333333 in this datatype.It is in general not of much
use because a long long isn't often translated in this format. If you
know that pushl is 0x68 followed by the long constant, you can also
search for 68 01 61 42 2E 68 55 55 4D 83 in hexadecimal/1. Or
search for 68 00776102145 68 -2092083883 in a composed datatype
consisting of hex/1 followed by signed-dec/4.
