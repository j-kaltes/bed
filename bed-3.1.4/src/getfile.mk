
#LOADLIBS=-lextra -ltermcap
LOADLIBS=-lncurses
#CFLAGS=-g -DCURSES -DTEST -Wall -D_GNU_SOURCE 
#CFLAGS=-g -DCURSES -DTEST3 -Wall -D_GNU_SOURCE 
#CFLAGS=-g -DCURSES -DTEST2
CFLAGS=-g -DCURSES -DTEST5 -DEENTEST
#CFLAGS=-g -DCONIO -DTEST
all: onlygetfile
%.o:%.cpp
	gcc -g -c $(CXXFLAGS)  $^

onlygetfile.o:getfile.cpp
	gcc -g -c $(CFLAGS)  getfile.cpp -o onlygetfile.o
onlygetfile: onlygetfile.o glob.o expandfilename.o  onlykey.o getlabel.o keyinput.o
	gcc  $(CFLAGS) $^ -o $@  $(LOADLIBS);

option: getfile.o glob.o expandfilename.o
	gcc  $(CFLAGS) $^ -o $@  $(LOADLIBS);
fileget.o: getfile.o glob.o
	gcc  $(CFLAGS) $^ -o $@  $(LOADLIBS);
