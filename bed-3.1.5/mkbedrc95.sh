cat <<!

#newfilesize 512 #gives the size of new files ( Alt-f,n)
context TERM rxvtbed
#don't place any keys statements before this statements
#the first keys statement initialises ncurses
setenv TERMINFO c:/Program Files/bed/terminfo
setenv isxterm rxvtbed
context TERM
datatypedir $CONFDIR/misc/
CONTEXT COLORTERM 
keys  27 91 55 126 = home #home
keys  27 91 56 126 = end #end
CONTEXT CURSES_VERSION PDCURSES
setenv PDCURSES 1
keys 26 = shell
keys 27 209 = consoleresize
setenv no_nconsole 1
!
cat vikeys
cat plugins/examples/ingive
cat <<"!"
CONTEXT CURSES_VERSION PDCURSES
setenv noxterm 1
context DISPLAY
setenv isxterm DISPLAY
context TERM xterm
setenv isxterm xterm
context TERM rxvt
setenv isxterm rxvt
context TERM xterm-color
setenv isxterm xterm-color
context ! isxterm
setenv noxterm 1
#color OPENLIST    black  red bold reverse
CONTEXT WINDOWS_PLATFORM WINDOWS
setenv noresize 1
CONTEXT  !noxterm
setenv noresize 1
context !noresize
menuitem 8 "Resi~ze" "F11" 1 + = consoleresize
CONTEXT ! noxterm 
setenv USERXVT yes
setenv no_nconsole 1
setenv SHELL bash.exe
color OPENLIST    black red bold
color OPENLISTACT  red  white bold reverse
color OPENLISTSEL  green  magenta
color OPENNORMAL  white  magenta bold 
color OPENLISTACT  brown yellow bold  reverse
color SPACEAFTER black white
color TEXT  normal
color SELECTION  reverse
color CURSOR  red   green bold reverse
color SELCURSOR  black cyan  reverse underline bold
color COMMAND reverse
color BAR BLUE white reverse
color BARKEY  black  white bold underline
color MENU  WHITE BLUE  bold
color MENUKEY  black white bold underline
color MENUSEL BLUE  WHITE
#color OPENTITLE yellow red   reverse bold
#color OPENTITLE red yellow  reverse bold
#color OPENKEY  yellow  BLACK bold reverse
color OPENTITLE yellow green  reverse bold
color OPENKEY   block white  bold underline
color SHOWBACK    white    blue reverse
keys  127 = backspace #backspace
keys  27 91 49 50 126 = dosave #F2
keys  27 91 49 51 126 = nextsearch #F3
keys  27 91 49 52 126 = playrecord #F4
keys  27 91 72 = home #home
keys  27 91 70 = end #end
keys  27 79 80 = f1 #F1
keys  27 91 49 49 126 = f1 #F1
keys  27 79 81 = dosave #F2
keys  27 79 82 = nextsearch #F3
keys  27 79 50 82 = beginfile #S-F3
keys  27 79 50 83 = endfile #S-F4
keys  27 91 49 56 59 50 126 = middlepage #S-F7
keys  27 91 49 53 59 50 126 = addprev #S-F5
keys  27 91 49 55 59 50 126 = addnext #S-F6
keys  27 79 83 = playrecord #F4
keys 27 91 49 59 53 70 = endfile
keys 27 91 49 59 53 72 = beginfile
keys 27 91 56 94 = endfile
keys 27 91 55 94  = beginfile
keys 27 127 = beginfile
keys 27 128 = endfile
keys  27 91 50 52 126 = setselect #F12
keys  27 91 50 48 126 = beginpage #F9
keys  27 91 50 49 126 = endpage #F10
#keys  27 10 = prevmark #M-Tab
keys  27 91 49 56 126 = prevmode #F7
keys  27 91 49 57 126 = nextmode #F8
keys  27 91 49 53 126 = prevfile #F5
keys  27 91 49 55 126 = nextfile #F6
context FROMXBED true
setenv USERXVT yes
call_on_start xbedmenu
context USERXVT
menuitem 8 "~Xterm menu" 3 + = xtermmenu
context ! no_nconsole 
# ncursus console 
keys 27 91 50 51 126 = consoleresize
keys  127 = backspace #backspace
keys  27 91 91 65 = f1 #f1
keys  27 91 91 66 = dosave #F2
keys  27 91 50 52 126 = setselect #F12
keys  27 91 91 67 = nextsearch #F3
keys  27 91 50 48 126 = beginpage #F9
keys  27 91 50 49 126 = endpage #F10
keys  27 91 49 56 126 = prevmode #F7
keys  27 91 49 57 126 = nextmode #F8
keys  27 91 91 69 = prevfile #F5
keys  27 91 49 55 126 = nextfile #F6
keys  27 91 91 68 = playrecord #F4

context ! isxterm
setenv nopdterm 1
context ! PDCURSES
setenv nopdterm 1
context ! nopdterm  # PDCURSES in xterm

keys  27 130 = up #up
keys  27 136 = down #down
keys  27 132 = left #left
keys  27 134 = right #right
keys  27 131 = pgup #pgup
keys  27 137 = pgdn #pgdn
keys  27 142 = delete #delete
keys  27 163 = nextmark #Tab

context ! COLORTERM
setenv noxpd 1
context ! PDCURSES
setenv noxpd 1
context ! noxpd
keys 27 104 = f1 # f1 doesn't work, use alt-h instead
color BAR red white reverse
color MENU  WHITE BLUE  bold
color OPENTITLE yellow red   reverse bold
color OPENKEY  yellow  BLACK bold reverse
color OPENLIST      white black bold reverse
context PATH
#general context

call_on_switch titleset
!
