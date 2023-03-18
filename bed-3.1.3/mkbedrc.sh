function inplug() {
   [[ "$INLINKALL" == "${INLINKALL/$1/}" ]]
}
if test $CONFIG_PLUGIN = y
then
cat <<!
#plugindir $CONFDIR/plugins/examples/
plugindir $CONFDIRLINK/plugins/examples/
!
fi
cat <<!
datatypedir $CONFDIRLINK/misc/
context !  TERMINFO 
setenv TERMINFO $CONFDIRLINK/terminfo
context TERM xterm-256color
setenv TERM xterm
#newfilesize 512 #gives the size of new files ( Alt-f,n)
# if COLORTERM=gnome-terminal and TERM=xterm then setenv TERM=rxvt
context ! COLORTERM gnome-terminal
setenv nognomexterm 1
context ! TERM xterm
setenv nognomexterm 1
context ! nognomexterm
setenv TERM rxvt
color OPENLIST    black  cyan bold
color OPENLISTACT  red  white bold reverse
color OPENLISTSEL  green  magenta
color OPENNORMAL  white  magenta bold 
#context TERM rxvtbed
#setenv TERMINFO $CONFDIRLINK/terminfo
#******
#***  DON'T PLACE ANY KEYS STATEMENTS BEFORE THIS STATEMENTS
#***  THE FIRST KEYS STATEMENT INITIALISES NCURSES
#*****
context TERM
!
cat <<"!"
# ****COLORS****
# SYNTAX:
# -> textpart foreground background attributes
# attributes -> []
# attributes -> attr attributes
# textpart  one of {TEXT COMMAND BAR BARKEY MENU MENUKEY MENUSEL OPENTITLE  OPENKEY OPENNORMAL OPENLIST OPENLISTSEL OPENLISTACT SHOWBACK SELECTION CURSOR}
# foreground -> color
# background -> color
# color oneof {BLACK RED GREEN YELLOW BLUE MAGENTA CYAN WHITE }
# attr oneof {NORMAL STANDOUT UNDERLINE REVERSE BLINK DIM BOLD PROTECT INVIS ALTCHARSET CHARTEXT}
#The following symbols can be bound to keys. This also effect the filemanager, the find/replace menu and the buffer and macro listings
#home, end, up, down, left, right, pgup, pgdn, stab, delete, backspace, esc 
keys  27 91 49 126 = home
keys  27 91 52 126 = end
keys  27 91 65 = up
keys  27 91 66 = down
keys  27 91 68 = left
keys  27 91 67 = right
keys  27 91 53 126 = pgup
keys  27 91 54 126 = pgdn
keys  27 91 51 126 = delete

keys  27 9 = stab
keys  127 = backspace
keys  27 254 = esc
keys  127 = backspace #backspace
keys  27 92 = scrollup #M-\
keys  27 124 = edscholldown #M-|
keys  27 91 27 91 = scrollitem #M-[,M-[
keys  27 93 = scrollbackitem #M-]
keys  27 123 = scrollbyte #M-{
keys  27 125 = scrollbackbyte #M-}
keys  27 59 = uplevel #M-;
keys  27 58 = downlevel #M-:
setenv isxterm rxvtbed
context DISPLAY
setenv isxterm DISPLAY
context TERM xterm
setenv isxterm xterm
context TERM rxvt
setenv isxterm rxvt
context TERM xterm-color
setenv isxterm xterm-color
CONTEXT isxterm

color OPENLISTACT  brown yellow bold  reverse
color SPACEAFTER black white
color TEXT  normal
color SELECTION  reverse
color CURSOR  red   green bold reverse
color SELCURSOR  black cyan  reverse underline bold

color COMMAND reverse
color BAR BLUE white reverse
color BARKEY  black  white bold underline

color MENU BLUE WHITE reverse
color MENUKEY  black white bold underline
color MENUSEL BLUE  WHITE
#color OPENTITLE yellow red   reverse bold
#color OPENTITLE red yellow  reverse bold
#color OPENKEY  yellow  BLACK bold reverse
color OPENTITLE yellow green  reverse bold
color OPENKEY   block white  bold underline
color SHOWBACK    white    blue reverse



keys  127 = backspace #backspace
keys  27 92 = scrollup #M-\
keys  27 124 = edscholldown #M-|
keys  27 91 27 91 = scrollitem #M-[,M-[
keys  27 93 = scrollbackitem #M-]
keys  27 123 = scrollbyte #M-{
keys  27 125 = scrollbackbyte #M-}
keys  27 59 = uplevel #M-;
keys  27 58 = downlevel #M-:
# not needed for rxvt
keys  27 91 72 = home #home
keys  27 91 70 = end #end

!
if test $HAS_X = y
then
#if test $CONFIG_LINKINPLUGINS = n
if inplug switchsize.plug
then
cat <<!
context XTERM_VERSION
plugin switchsize.plug
context COLORTERM rxvt
plugin switchsize.plug
!
fi
cat <<!
context XTERM_VERSION
keys 27 91 50 51 126 = switchsize
context COLORTERM rxvt
keys 27 91 50 51 126 = switchsize
!
fi
cat <<!

CONTEXT TERM #general context
keys  27 91 27 91  = scrollitem
keys  27 123 = scrollbyte
keys  27 93 = scrollbackitem
keys  27 125 = scrollbackbyte
keys  27 92 = scrollup
keys  27 124 =  edscholldown
keys 27 80 =  f1
keys 27 79 80 =  f1
CONTEXT TERM vt100 
# when trying to use bed with win95 telnet  type 
#export LINES=24;export COLUMNS=78;export TERM=vt100
#You can use Alt-key combination by typing very fast ESC followed by key
keys  27 27  = tocommand
call_on_start tocommand
context TERM
#context TERM linux
#other kb  cub1  le 
keys kf15 = beginfile
keys kf16 = endfile
keys kf17 = addprev
keys kf18 = addnext
keys kf19 = middlepage
CONTEXT  isxterm
keys kf13 = beginfile
keys kf14 = endfile
keys kf15 = addprev
keys kf16 = addnext
keys kf17 = middlepage
context ! nognomexterm
# apply if shell variable TERM=rxvt
keys  27 79 72 = home #home
keys  27 79 70 = end #end
keys  27 79 49 59 50 82 = beginfile #S-F3
keys  27 79 49 59 50 83 = endfile #S-F4
CONTEXT TERM
keys kbs =  backspace 
keys kfnd = home # rxvt
keys kslt = end # rxvt
keys home = home # xterm only
#keys end = end # xterm only
keys khome =  home # linux
keys kend = end # linux
#keys cr = input # return
keys kpp =  pgup
keys knp =  pgdn
keys cuf1  = right # right key
keys kcub1 = left # idem xterm,linux,rxvt alternative: kll 
keys cuu1 = up # up alternative: kcuu1 
keys kcud1 = down # down
#keys kdch1 = delete # delete
keys 27 58 = downlevel
keys 27 59 = uplevel
context TERM cons25
setenv CONSOLETERM yes
context TERM cons30
setenv CONSOLETERM yes
context TERM cons50
setenv CONSOLETERM yes
context TERM cons43
setenv CONSOLETERM yes
context TERM cons60
setenv CONSOLETERM yes
CONTEXT CONSOLETERM yes
keys  27 91 97 = beginfile #S-F3
keys  27 91 98 = endfile #S-F4
keys  27 91 101 = middlepage #S-F7
keys  27 91 99 = addprev #S-F5
keys  27 91 100 = addnext #S-F6
CONTEXT isxterm
 # Needed if you use konsole with TERM=xterm-color
keys  27 79 65 = up #up
keys  27 79 67 = right #right
#keys  63 = stab #stab ? what alternative
keys  27 79 50 82 = beginfile #S-F3
keys  27 79 50 83 = endfile #S-F4
keys  27 91 49 56 59 50 126 = middlepage #S-F7
#keys  63 = prevmark #M-Tab What alternative?
keys  27 91 49 53 59 50 126 = addprev #S-F5
keys  27 91 49 55 59 50 126 = addnext #S-F6

keys 27 91 55 126   = home #rxvt
keys 27 91 56 126 = end #rxvt
keys  27 91 70 = end #end

#keys  27 91 50 52 36 = stab #stab
keys  27 91 49 50 126 = dosave #F2
keys  27 91 49 51 126 = nextsearch #F3
keys  27 91 50 52 36 = prevmark #M-Tab
keys  27 91 49 52 126 = playrecord #F4
keys 27 91 54 94 = endfile
keys 27 91 53 94 = beginfile
keys 27 91 54 59 53 126 = endfile
keys 27 91 53 59 53 126 = beginfile
#CONTEXT TERM rxvt # Needed if you use konsole with TERM=rxvt
keys  27 79 66 = down #down
keys  27 79 68 = left #left
keys  27 79 81 = dosave #F2
keys  27 79 82 = nextsearch #F3
keys  27 79 83 = playrecord #F4
context FROMXBED true
setenv USERXVT yes
call_on_start xbedmenu
#call_on_start switchmenu
CONTEXT TERM #general context
#call_on_start record
!
if test  .$CONFIG_THREADS = .y -a $HAS_X = y  
then
cat <<"!"
context DISPLAY
!
#if test $CONFIG_LINKINPLUGINS = n
if inplug putselect.plug
then
cat <<"!"
plugin putselect.plug
!
fi
cat <<"!"
menuitem 1 "~Select" "F12" 3  = makeselection
keys  kf12 = makeselection
menuitem 1 "~Copy" "C-y" 4  =copyclipboard
keys 25 = copyclipboard
menuitem 1 "~Past" "M-p" 5  = pastclipboard 
keys  27 112 = pastclipboard
menuitem 1 "Past~ Special" 6 +  = pastspecial
menuitem 1 LINE 8 + 
menuitem 1 "Past Se~lected" 9 +  = pastselection 
!
fi
cat <<"!"
context COLORTERM rxvt
setenv USERXVT yes
context COLORTERM rxvt-xpm
setenv USERXVT yes
context USERXVT
!

#if test $CONFIG_LINKINPLUGINS = n
if inplug rxvt.plug
then
cat <<"!"
plugin rxvt.plug
!
fi
cat <<"!"

menuitem 8 "~Xterm menu" 3 + = xtermmenu
CONTEXT TERM rxvt
# apply if shell variable TERM=rxvt
keys  27 91 50 48 126 = beginpage #F9
keys  27 91 50 49 126 = endpage #F10
keys  27 91 49 56 126 = prevmode #F7
keys  27 91 49 57 126 = nextmode #F8
#keys   = addprev #S-F5
keys  27 91 49 53 126 = prevfile #F5
keys  27 91 49 55 126 = nextfile #F6
!

cat vikeys
cat plugins/examples/ingive
cat <<"!"
menuitem 2 "~Find copybuf" 3 + = searchclip
menuitem 6 "S~how info" 5 + = datatypeinfo

#setenv PAGER #unset PAGER for bed 
#setenv PAGER less #use less as PAGER for bed only
#setenv LINES_USE_PAGER 20 
#Set by how many lines you want to use the pager set by environmental 
#variable PAGER instead of the internal pager. 
#setenv LINES_USE_PAGER 20 

call_on_switch titleset
!
