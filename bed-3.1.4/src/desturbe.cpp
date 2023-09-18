#include "defines.h"
#include <sys/types.h>
#include <signal.h>

#include "desturbe.h"
extern int pid;
extern void stopmain(void) ;
extern void contmain(void) ;
//#ifdef USETHREADS
#ifdef USE_LOCKDESTURBE // Doesn't seems to be nessessary to me
#ifndef SIG_SEMAPHORE
pthread_mutex_t desturbemutex;
pthread_mutexattr_t desturbemutexattr;
	void stopmain(void) {
		DESTLOCK( );
		}
	void contmain(void) {
		DESTUNLOCK( );
		}
#else
#if __GNUC__
#warning "stopmain and contmain not implemented if SIG_SEMAPHORE defined"
#endif
	void stopmain(void) {
		}
	void contmain(void) {
		}
#endif
#else
	void stopmain(void) {
		INCDEST();
		}
	void contmain(void) {
		DECDEST();
		}

#endif
int dontdesturbe=0;
