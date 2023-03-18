#ifndef DESTURBE_H
#define DESTURBE_H
extern int dontdesturbe;
/*#ifdef USETHREADS*/
#ifdef USE_LOCKDESTURBE
#ifdef SIG_SEMAPHORE 
#include <semaphore.h>
extern sem_t desturbesem;
inline void DESTLOCK(void ) {sem_wait(&desturbesem);}
inline void DESTUNLOCK(void) {sem_post(&desturbesem);}
inline void ENDDEST(void) {sem_destroy(&desturbesem);}
inline	void INITDEST(void) {sem_init(&desturbesem,0,1);}
#else
#define __GNU_VISIBLE 1
#include <pthread.h>
extern pthread_mutex_t desturbemutex;
extern	pthread_mutexattr_t desturbemutexattr;
inline void DESTLOCK(void ) { pthread_mutex_lock(& desturbemutex);}

inline void DESTUNLOCK(void) { pthread_mutex_unlock(& desturbemutex);}
inline void ENDDEST(void) {
       pthread_mutex_destroy(&desturbemutex);
	pthread_mutexattr_destroy(&desturbemutexattr);

}

inline	void INITDEST(void) {
	pthread_mutexattr_init(&desturbemutexattr);
	pthread_mutexattr_settype(&desturbemutexattr,MY_PTHREAD_MUTEX_ERRORCHECK);
     pthread_mutex_init(&desturbemutex,   &desturbemutexattr);

	}
#endif
inline void INCDEST(void) {
	if(dontdesturbe++<=0) {
		dontdesturbe=1;
		DESTLOCK();
		}
	}
inline void DECDEST(void) {
		if((--dontdesturbe)<=0)  {
			dontdesturbe=0;
			DESTUNLOCK();
			}
	}
inline void NODEST(void) {if(dontdesturbe) DESTUNLOCK(); dontdesturbe=0;}
inline void SETDEST(int num) {if(num!=dontdesturbe) { if(dontdesturbe<=0) DESTLOCK(); else { if(!num) DESTUNLOCK(); }; dontdesturbe=num; };}
inline int GETDEST(void) {return dontdesturbe;}
inline int ISDEST(void) {return dontdesturbe;}

#else
#define INCDEST() dontdesturbe++
#define DECDEST() dontdesturbe--
#define NODEST()  dontdesturbe=0
#define SETDEST(num)  dontdesturbe=num
#define GETDEST() dontdesturbe
#define	ISDEST() dontdesturbe
#define ENDDEST()
#define INITDEST() 

#endif
#define INCPDDEST INCDEST  
#define DECPDDEST() DECDEST()
#define NOPDDEST() NODEST()
#define SETPDDEST(num)  SETDEST(num)
#define GETPDDEST() GETDEST()
#define ISPDDEST() ISDEST()
#define ENDPDDEST() ENDDEST()
#define INITPDDEST() INITDEST()



#define noop (void)0
extern int wantsignal;
#define signalwant(onleave,ret) {\
		if(wantsignal) {\
			onleave;\
			int olddont=GETDEST();\
			NODEST();\
			int wassig=wantsignal;\
			wantsignal=0;\
			kill(pid,wassig);	\
			SETDEST(olddont);\
			return ret;\
			}\
	}
#endif

