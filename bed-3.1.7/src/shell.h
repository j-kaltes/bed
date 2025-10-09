#ifndef __SHELL_H__
#define __SHELL_H__
#ifdef HAVE_SYS_WAIT_H 
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <stdarg.h>
#include "varmacro.h"

template<typename T,typename  ... Ts>
int  process(T com,Ts... args)  {
        int statusdieprocessreturns;
        return (fork()?(wait(&statusdieprocessreturns),statusdieprocessreturns):( (execlp(com,com,args...,NULL), perror(com)), exit(4),1));
        }

#else
#include "strconcat.h"
/*
template<typename T,typename  ... Ts>
void  procbuf(char *buf,T one,Ts... args)  {
	int len=strlen(one);
	memcpy(buf,one,len);
	 if constexpr (sizeof...(args)) {
		buf[len++]=' ';
		procbuf(buf+len,args...);
		}
	else
		buf[len]='\0';
	
        }
template<typename  ... Ts> int process(Ts ... args) {
	char buf[1024];
	procbuf(buf,args...);
	return system(buf);
	}
	*/

template<typename  ... Ts> int process(Ts ... args) {
	strconcat command(string_view(" "),args ...);
	return system(command.data());
	}
#endif
        

#endif
