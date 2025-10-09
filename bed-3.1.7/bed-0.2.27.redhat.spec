Summary: Adjustable data format binary editor
Name: bed
Version: 0.2.27
License: GPL
Release: 1
Source: sunsite.unc.edu:/pub/Linux/apps/editors/terminal/bed-0.2.27.src.tgz 
Url: http://bed.dse.nl
Group: Applications/Editors
%description 
Adjustable data format binary editor. Data formats are
ASCII, unsigned and signed integers, float, bit-flags,
bit-fields, labels, ebcdic and time_t. Different sizes
and byte orderings are possible. Data types can
be used in structures. Other data formats, filters and
procedures can be defined in plugins. Contains copy,
past, undo, redo, search, replace, marks, record/play and
context sensitive help. Raw edit of hard drives. Under  
Linux and FreeBSD even usable without X windows.
%prep
%setup

%build
make realclean
#make linuxconfig
#s/^[^%]*%_\([^ 	]*\)[ 	].*$/--\1=%{_\1} /g
./configure       --prefix=%{_prefix} --exec_prefix=%{_exec_prefix} --bindir=%{_bindir} --datadir=%{_datadir} --libdir=%{_libdir} --mandir=%{_mandir} 
make dep
make
%install
########if test -e /usr/lib/bed-0.2.27/uninstallbed.sh; then bash /usr/lib/bed-0.2.27/uninstallbed.sh --force ; fi
make  installfiles ROOTDIR=$RPM_BUILD_ROOT
%post
ln -T -fs /usr/lib/bed-0.2.27 /usr/share/bed
cd /usr/lib/bed-0.2.27
bash ./addmagic
if  which update-menus 2>/dev/null; then update-menus; fi
%files
%config /usr/lib/bed-0.2.27
/usr/share/bed
/usr/bin/bed
/usr/share/man/man1/bed.1.gz
/usr/share/applications/bed-binary-editor.desktop 


