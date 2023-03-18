#ifndef NEPSEMA_H

#define NEPSEMA_H

#ifdef HAS_SEMAPHORE
#include <semaphore.h>
#else
#undef HAS_GETVALUE
#define sem_init(x,y,z) pthread_mutex_init(x,NULL)
#define sem_wait(x) pthread_mutex_lock(x)
#define sem_post(x) pthread_mutex_unlock(x)
#define sem_trywait pthread_mutex_trylock
#define	sem_t pthread_mutex_t
#define sem_destroy pthread_mutex_destroy
#endif
#endif
