AC_INIT([bed],[3.1.3],[binaryeditor@gmx.com])
#AC_INIT([bed],[$VERSION],[binaryeditor@gmx.com])
AC_CONFIG_HEADERS([src/confdefines.h])
. ./settings
AC_CANONICAL_HOST
AC_CANONICAL_BUILD

AC_PROG_CC
AC_PROG_CXX
AC_CHECK_TOOLS([STRIP],[llvm-strip strip])
if $STRIP -V|grep  GNU
then
	GNUSTRIP=yes
else
	GNUSTRIP=no
fi
AC_PATH_PROG(MAN2HTML,man2html)
if test .$MAN2HTML != . && $MAN2HTML -Hwww.test.org -M /a/path/ < man2html/man2html.1|grep -q '<B><A HREF="http://www.test.org/a/path/?1+man">man</A></B>(1)'
then
	echo found, use system version.
else
	MAN2HTML=../man2html/man2html
	echo not found, use included version.
fi

CYGWIN=no
SOLARIS=no
case $host_os in
    *android*) SYSTEM=Android;;
    *linux* ) SYSTEM=Linux;;
    *freebsd* ) SYSTEM=FreeBSD;;
    *solaris* ) SYSTEM=SunOS;SOLARIS=yes;;
    *cygwin* ) CYGWIN=yes;SYSTEM=CYGWIN;;
esac
echo Host os for bed $host_os
echo Host cpu $host_cpu
if test .$SOLARIS = .yes
then
export LDFLAGS="-L/usr/gnu/lib -R/usr/gnu/lib"
CONFIG_LINKINPLUGINS=n
else
CONFIG_LINKINPLUGINS=y
fi

if test .$CYGWIN = .yes
then
CYGWINDLL=2000
if test .$cross_compiling = .no
then
AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <stdio.h>
#include <sys/cygwin.h>
#include <cygwin/version.h>
int main(void) {
	int get= CYGWIN_VERSION_DLL_MAJOR;
	FILE *file=fopen("conftest.printzd","w");
	if(!file)
		return(2);
	fprintf(file,"%d\n",get);
	fclose(file);
	return(0);
}
]])],[CYGWINDLL=`cat conftest.printzd` ],[],[])
echo CYGWIN_VERSION_DLL_MAJOR=$CYGWINDLL
else
echo "cross compiling, assume CYGWIN_VERSION_DLL_MAJOR > 1007"
fi # cross

fi # CYGWIN

AC_LANG(C)
AC_LANG(C++)
#AC_PROG_CPP
#AC_PROG_CXXCPP
AC_PROG_EGREP
#AC_PROG_INSTALL

AC_CHECK_SIZEOF(long)
AC_CHECK_TYPES([off64_t,__off64_t])

#OLDLIBS=$LIBS
#AC_CHECK_FUNC(GetWindowsDirectoryA ,WINDOWS=yes,WINDOWS=no)
AC_CHECK_FUNC(GetStdHandle,WINDOWS=yes,WINDOWS=no)
if test .$WINDOWS = .no
then
AC_CHECK_LIB(kernel32,GetStdHandle,WINDOWS=yes,WINDOWS=no)
if test .$WINDOWS = .no
then
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <windows.h>
]], [[{
GetStdHandle(STD_OUTPUT_HANDLE) ;
}]])],[WINDOWS=yes],[WINDOWS=no])
fi

fi

AC_CHECK_FUNCS(strchrnul setenv ftruncate64 truncate64 open64 stat64 fstat64 glob stpcpy sendfile sendfile64 bzero sigsuspend sigaddset memccpy kill  tcsetattr)



AC_CHECK_HEADERS(curses.h ncurses.h ncurses/ncurses.h gnuregex.h regex.h float.h sys/errno.h errno.h sys/ioctl.h dirent.h sys/wait.h termios.h sys/mman.h re2/re2.h)


AC_CHECK_LIB(magic, magic_buffer, HAVE_MAGIC_H=1)
if test   .$HAVE_MAGIC_H != .1
then
	AC_CHECK_LIB(z, magic_buffer,HAVE_MAGIC_H=1; LIBS+=-lz,true,-lmagic)
fi
if test .$HAVE_MAGIC_H = .1
then
		AC_SUBST(HAVE_MAGIC_H,1)
fi

#AC_CHECK_HEADER(magic.h,HAVE_MAGIC_H=1)
#if test .$HAVE_MAGIC_H = .1
#	then
#		AC_SUBST(HAVE_MAGIC_H)
#		AC_DEFINE(HAVE_MAGIC_H)
#	fi
AC_EGREP_HEADER(tigetstr,$CURSESLIB.h,HAS_TIGETSTR_HEADER=1,unset HAS_TIGETSTR_HEADER)

AC_CHECK_LIB($CURSESLIB,PDC_ungetch,AC_DEFINE(USE_KEYPAD) AC_DEFINE(PDCURSES) PDCURSES=1,)
AC_CHECK_LIB($CURSESLIB,assume_default_colors,true,AC_DEFINE(NODEFAULTCOLORS))
AC_CHECK_LIB($CURSESLIB,delscreen,AC_DEFINE(HASDELSCREEN),)
AC_CHECK_LIB($CURSESLIB,curs_set,AC_DEFINE(HAS_CURS_SET),)
AC_CHECK_LIB($CURSESLIB,start_color,CONFIG_COLOR=y,CONFIG_COLOR=n)
AC_CHECK_LIB($CURSESLIB,wresize,AC_DEFINE(HAS_WRESIZE),)
AC_CHECK_LIB($CURSESLIB,tigetstr,[
if test $HAS_TIGETSTR_HEADER
then
echo HAS_TIGETSTR_HEADER and HAS_TIGETSTR
AC_DEFINE(HAS_TIGETSTR)
fi
],)
#AC_FUNC_MMAP
OLDLIBS=$LIBS
AC_SEARCH_LIBS(dlopen,dl, CONFIG_PLUGIN=y,CONFIG_PLUGIN=n)
LIBS=""
AC_CHECK_LIB(dl,dlopen)
DLLIBS=$LIBS
AC_SEARCH_LIBS(lseek64,c,CONFIG_LARGEFILES=y;AC_DEFINE(HAS_LSEEK64),CONFIG_LARGEFILES=n)
echo -n "Checking for sigsetjmp ... "
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <setjmp.h>
]], [[{
sigjmp_buf  jmp;
if(sigsetjmp(jmp,2)==4) { }
}]])],[echo yes;AC_DEFINE(HAS_SIGSETJMP)],[echo no]) 
#AC_SEARCH_LIBS(__sigsetjmp,c gcc_s ,AC_DEFINE(HAS_SIGSETJMP))

AC_CHECK_SIZEOF(off_t)
if test .$CONFIG_LARGEFILES = .n
then

EXTRAFLAGS="$EXTRAFLAGS -DSIZEOF_OFF_T=$ac_cv_sizeof_off_t" 

if test -n $ac_cv_sizeof_off_t
then
	AC_SUBST(SIZEOF_OFF_T)
	if test $ac_cv_sizeof_off_t -ge 8
	then
	CONFIG_LARGEFILES=y
	fi
fi
fi

AC_CHECK_SIZEOF(ssize_t)
AC_CHECK_SIZEOF(long double)

#m4_include([m4-ax_printf_size_t.m4])

#AX_PRINTF_SIZE_T


AC_EGREP_CPP(yes234,
     [
#include <sys/mman.h>
#if defined(MAP_FAILED)
       yes234
     #endif
     ], AC_DEFINE(HAS_MAP_FAILED))

AC_SEARCH_LIBS(mremap,c,AC_DEFINE(HAS_MREMAP))
AC_SEARCH_LIBS(mkstemp,c,AC_DEFINE(HAS_MKSTEMP))
AC_SEARCH_LIBS(strnlen,c,AC_DEFINE(HAS_STRNLEN))
AC_CHECK_LIB(c,getusershell,AC_DEFINE(HAS_GETUSERSHELL),)
AC_CHECK_LIB(c,gmtime_r,AC_DEFINE(HAS_GMTIME_R))
AC_CHECK_LIB(c,localtime_r,AC_DEFINE(HAS_LOCALTIME_R))
AC_CHECK_LIB(c,strtoll,AC_DEFINE(HAS_STRTOLL),)
AC_CHECK_LIB(c,pread,AC_DEFINE(HAS_PREAD),)
#AC_CHECK_LIB(c,copy_file_range,AC_DEFINE(HAS_COPY_FILE_RANGE),)

if test .$SYSTEM != .Android
then
	AC_CHECK_LIB(c,ttyname_r,AC_DEFINE(HAS_TTYNAME_R),)
fi
AC_CHECK_LIB(c,realpath,AC_DEFINE(HAS_REALPATH),)


AC_CHECK_FUNCS_ONCE(timegm)

echo -n 'Checking for tm_gmtoff... '
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <stdio.h>
#include <time.h>
]], [[{
struct tm tim;
printf("%d",tim.tm_gmtoff);
}
 ]])],[echo yes],[echo no;TIMEZONENEEDED=1; AC_DEFINE(TIMEZONENEEDED)])
if test .$TIMEZONENEEDED = .1
then
echo -n 'Checking for int timezone... '
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <time.h>
#include <stdio.h>
]], [[{
time_t x;
x+=timezone;
}
]])],[echo yes;AC_DEFINE(NOTIMEZONE)],[echo no])
fi

AC_LANG(C++)
AC_CHECK_LIB(c,strptime,,AC_DEFINE(OWNTRANSTIME))


#AC_COMPILE_IFELSE( #define _XOPEN_SOURCE   #include <time.h> int main(void) { struct tm uit;	char formal[100]; strptime(formal,"%a %b %e %H:%M:%S " , &uit); return 0; } ,,AC_DEFINE(OWNTRANSTIME)) 




AC_SEARCH_LIBS(clock_gettime,rt,AC_DEFINE(HAVE_CLOCK_GETTIME))
LIBS=$OLDLIBS
AC_SEARCH_LIBS(xre_search_2,iberty,HAS_RE_SEARCH=y,HAS_RE_SEARCH=n)
#AC_SEARCH_LIBS(re_search_2,gnuregex,HAS_RE_SEARCH=y,HAS_RE_SEARCH=n)
if test $HAS_RE_SEARCH = n
then
if test -d regex-0.13
then
AC_CONFIG_SUBDIRS([regex-0.13])
fi
fi
OLDLIBS=$LIBS
LIBS=""
AC_SEARCH_LIBS(hs_scan_stream ,hs , USE_HYPERSCAN=1 )
LIBHS=$LIBS
#AC_CHECK_HEADER(re2/re2.h,USE_RE2=1 )

LIBS=""
AC_SEARCH_LIBS(_ZN3re23RE213PartialMatchNERKNS_11StringPieceERKS0_PKPKNS0_3ArgEi ,re2 , HAVE_RE2_RE2_H=1 )
LIBRE2=$LIBS
LIBS=""
AC_SEARCH_LIBS(pthread_create,pthread c_r,CONFIG_THREADS=y,CONFIG_THREADS=n)
echo  CONFIG_THREADS=$CONFIG_THREADS 
if test $CONFIG_THREADS = y
then
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <pthread.h>
]], [[{
	pthread_mutexattr_t mut;
	pthread_mutexattr_init(&mut);
	pthread_mutexattr_settype(&mut,PTHREAD_MUTEX_ERRORCHECK);
	}
]])],[AC_DEFINE(MY_PTHREAD_MUTEX_ERRORCHECK,PTHREAD_MUTEX_ERRORCHECK)],[])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <pthread.h>
]], [[{
	pthread_mutexattr_t mut;
	pthread_mutexattr_init(&mut);
	pthread_mutexattr_settype(&mut,PTHREAD_MUTEX_ERRORCHECK_NP);
	}
]])],[AC_DEFINE(MY_PTHREAD_MUTEX_ERRORCHECK,PTHREAD_MUTEX_ERRORCHECK_NP)],[])

AC_SEARCH_LIBS(sem_wait,pthread c_r,AC_DEFINE(HAS_SEMAPHORE))
AC_SEARCH_LIBS(sem_getvalue,pthread c_r,AC_DEFINE(HAS_GETVALUE))

fi

THREADSLIB=$LIBS
LIBS=$OLDLIBS


echo -n 'checking for sysconf(_SC_AVPHYS_PAGES)... '
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <unistd.h>
]], [[long psize;
long av;
long max;
psize=sysconf(_SC_PAGESIZE);
max=sysconf(_SC_PHYS_PAGES);
av=sysconf(_SC_AVPHYS_PAGES);
return(0);
 ]])],[echo yes;AC_DEFINE(SYSCONF_MEM)],[echo no])

AC_SEARCH_LIBS(log,m)


RXVTS="rxvt rxvt.bin rxvt.sh rxvt-xpm rxvt-xterm"
RXVT=""
NRXVT=""
CONFIG_XBED=n
if test .$PDCURSES != .1
then
for i in $RXVTS
do
	if $i -h 2>&1 | grep -qi menubar
	then
		CONFIG_XBED=y
		RXVT="$RXVT $i"
	else
		NRXVT="$NRXVT $i"
	fi
done
if test .$CONFIG_XBED != .y
then
		RXVT=rxvt
fi
fi
AC_CHECK_PROG(CONFIG_PROGMAN,update-menus,y,n)
AC_CHECK_PROG(HAS_FILE,file,y,n)
AC_CHECK_PROG(HAS_NROFF,nroff,y,n)
if test .$HAS_FILE = .y
then
echo $CC $CPPFLAGS $CFLAGS $LDFLAGS conftest.c -o conftest
echo -n "Checking ELF format ..."
cat > conftest.c <<!
int main(void)		{
}
!
ELFSIZE=unknown
$CC $CPPFLAGS $CFLAGS $LDFLAGS conftest.c -o conftest
ELFFILEUIT=`file conftest`
case "$ELFFILEUIT" in 
  *[[^A-Za-z]]ELF[[^A-Za-z]]*)   # double [[ because of autoconf
	echo -n ' yes '
	USES_ELF=y
	case "$ELFFILEUIT" in 
	 *\(x86_\)*)
	   ;&
	 *\(ARM\)*)
	   ;&
	  *[[^0-9]]32[[^0-9]]*)
		ELFSIZE=32
	    ;;
	  *[[^0-9]]64[[^0-9]]*)
		ELFSIZE=64
	    ;;
	    esac
	echo $ELFSIZE bits
    ;;
    *)
    echo no
	USES_ELF=n
esac
rm -f conftest
fi
#if test .$SYSTEM = .Linux
#then
#	AC_DEFINE(PROCARRAY)
#fi

#	AC_DEFINE(NOSTOPANDTHREADS)
AC_PROG_LN_S
if $LN_S -fr $0 conftest 
then
rm conftest
AC_SUBST(HAVE_LN_R,y)
else
AC_SUBST(HAVE_LN_R,n)
fi
AC_PATH_XTRA
echo nox: $no_x
if test .$no_x = .yes
then
	HAS_X=n
else
	HAS_X=y
fi
AC_LANG([C++])


AC_LANG(C++)
#m4_include([ax_cxx_check_lib.m4])
#AX_CXX_CHECK_LIB(stdc++,[Std::set_new_handler],  AC_DEFINE(HAS_SETNEWHANDLER))
#AX_CXX_CHECK_LIB(c,[Std::set_new_handler],  AC_DEFINE(HAS_SETNEWHANDLER))
#AX_CXX_CHECK_LIB(gcc_s,[Std::set_new_handler],  AC_DEFINE(HAS_SETNEWHANDLER))
#AC_LANG(C++)
#AC_SEARCH_LIBS(Std::set_new_handler, stdc++ c gcc_s, AC_DEFINE(HAS_SETNEWHANDLER))


AC_CHECK_HEADER(new,AC_DEFINE(HAVE_NEW))
if test "x$ac_cv_header_new" = xyes
 then 
echo -n "Checking for std::set_new_handler ... "
cat > conftest.cc <<!
#include <new>

void mynewhandler(void) {
        }

int main(void)
 {
std::set_new_handler(mynewhandler);
return(0);
}
!
else
echo -n "Checking for set_new_handler ... "
cat > conftest.cc <<!
#include <new.h>
void mynewhandler(void) {
	}
int main(void)
 {
set_new_handler(mynewhandler);
return(0);
!
fi
if $CXX $CPPFLAGS $CXXFLAGS  $LDFLAGS conftest.cc -o conftest
then
AC_DEFINE(HAS_SETNEWHANDLER)
echo yes
else
echo no
fi



rm conftest.cc conftest
# Don't use c++14 because of "warning: taking sizeof array of runtime bound [-Wvla]"
# and sizeof returns 1
for GXXVERSION in c++20 c++2a c++17 c++1z gnu++1z c++14 c++1y gnu++14 gnu++1y c++11
do
echo -n "$GXXVERSION: "
if  test -z "`$CXX -std=$GXXVERSION -x c++ - 2>&1 <<!
int main(void) {
	int x=20;
	char  buf[[x]];
	return sizeof(buf);
	}
!
`"
then
break
fi
done
echo take $GXXVERSION 


function findprogenk() {
	try=g$prog
	if hash $try
	then 
		retval=$try
	else
		if test "$iscpp" = "+"
		then
			retval=clang++	
		else
			retval=clang
		fi
	
	fi
	}
function  findprog() {
	prog=$1
	iscpp="${prog:1:1}"
	if test -z "$build_alias"
	then
		findprogenk 
	else
		if test "$iscpp" = "+"
		then
			try=$build_alias-c$prog
		else
			try=$build_alias-$prog
		fi	
		if hash $try
		then
				retval=$try
		else
			try=$build_alias-g$prog
			if hash $try
			then
					retval=$try
			else
				findprogenk
			fi
		fi
	fi
	echo FOR BUILD ARCH $retval
	}





AC_CHECK_HEADER(features.h,,echo fea;EXTRAFLAGS="$EXTRAFLAGS -I\$(BEDSRCDIR)/utils")
AC_CHECK_HEADER(alloca.h,,echo allo;EXTRAFLAGS="$EXTRAFLAGS -I\$(BEDSRCDIR)/x11")

HOSTCONF="conf-$host_alias.h"
AC_SUBST(CROSS_COMPILING,$cross_compiling)
AC_DEFINE_UNQUOTED(HOST_ALIAS,"$host_alias")
AC_DEFINE_UNQUOTED(BUILD_ALIAS,"$build_alias")
AC_DEFINE_UNQUOTED(HOSTCONF,"$HOSTCONF")
AC_SUBST(HOSTCONF)
AC_SUBST(HOST_ALIAS,$host_alias)
AC_SUBST(HOST_OS,$host_os)
AC_SUBST(GXXVERSION)


findprog cc
AC_SUBST(TOBUILDCC,$retval)
findprog ++
AC_SUBST(TOBUILDCXX,$retval)


AC_SUBST(host_cpu)
AC_SUBST(SIZEOF_SSIZE_T)
AC_SUBST(PRI_SIZE_T_MODIFIER)
AC_SUBST(HAS_APPLICATIONS)
AC_SUBST(HAVE_OFF64_T)
AC_SUBST(HAS_X)
AC_SUBST(X_CFLAGS)
AC_SUBST(X_EXTRA_LIB)
AC_SUBST(X_PRE_LIBS)
AC_SUBST(X_LIBS)
AC_SUBST(SYSTEM)
AC_SUBST(WINDOWS)
AC_SUBST(USES_ELF)
AC_SUBST(ELFSIZE)
AC_SUBST(HAS_FILE)
#AC_SUBST(HAS_MAKEDEPEND)
AC_SUBST(CONFIG_XBED)
AC_SUBST(RXVT)
AC_SUBST(NRXVT)
AC_SUBST(CONFIG_PROGMAN)
AC_SUBST(HAS_RE_SEARCH)
AC_SUBST(CONFIG_PLUGIN)
AC_SUBST(CONFIG_THREADS)
AC_SUBST(USE_HYPERSCAN)
AC_SUBST(LIBHS)
AC_SUBST(LIBRE2)
AC_SUBST(USE_RE2,$HAVE_RE2_RE2_H)
AC_SUBST(CONFIG_LARGEFILES)
AC_SUBST(CONFIG_COLOR)
AC_SUBST(DYNAMICCURSES)
AC_SUBST(STATICCURSES)
AC_SUBST(CURSESLIB)
AC_SUBST(EXTRALIBS)
#AC_SUBST(VERSION)
#AC_SUBST(LOWVERSION)
#AC_SUBST(MIDVERSION)
#AC_SUBST(UPVERSION)
AC_SUBST(PROGRAMC)
AC_SUBST(PROGRAMLD)
AC_SUBST(CXX)
AC_SUBST(CC)
AC_SUBST(CXX)
AC_SUBST(LIBS)
AC_SUBST(DLLIBS)
AC_SUBST(THREADSLIB)
AC_SUBST(EXTRAFLAGS)
AC_SUBST(CYGWIN)
AC_SUBST(GNUSTRIP)
AC_SUBST(STRIP)
AC_SUBST(CYGWINDLL)
AC_SUBST(PDCURSES)
AC_SUBST(CONFIG_LINKINPLUGINS)
AC_SUBST(MAN2HTML)
AC_CONFIG_FILES([config pkg/Makefile man2html/Makefile])
AC_OUTPUT

./buildinfo/configure
echo $CXX  $PROGRAMC $CPPFLAGS $CXXFLAGS  $PROGRAMLD $LDFLAGS hosttest.cc reversetime.cc -o hosttest
$CXX  $PROGRAMC $CPPFLAGS $CXXFLAGS  $PROGRAMLD $LDFLAGS hosttest.cc reversetime.cc -o hosttest

if test .$cross_compiling != .no
then
echo "****Cross Compiling: certain information about future host of bed not known. Run ./hosttest on the host to get this information.	****"
echo "****information will be saved in $HOSTCONF										****"
echo "****put the generated file $HOSTCONF in  directory ./src 								****"
else
cd src;../hosttest
fi

