
#ifndef __MINGW32__
#include <sys/times.h>
#endif
 #include <unistd.h>

#include "debug.h"

#include <time.h>
#include <stdio.h>
extern unsigned long long startuptime,showmarkswhole; 
unsigned long long startuptime=8000000, showmarkswhole;
#ifdef _WIN32
#include <windows.h>
//#include <Realtimeapiset.h>
#include <realtimeapiset.h>
#ifdef  __cplusplus
extern "C" {
#endif
 WINBASEAPI WINBOOL WINAPI QueryProcessCycleTime (HANDLE ProcessHandle, PULONG64 CycleTime);
#ifdef  __cplusplus
}
#endif
extern LARGE_INTEGER  FrequencyPerformanceQuery;
LARGE_INTEGER  FrequencyPerformanceQuery;
void getstarttime(void) {
	QueryPerformanceFrequency(&FrequencyPerformanceQuery); 
	if(!QueryProcessCycleTime( GetCurrentProcess(),  &startuptime))  {
		startuptime=8000002;
		}
#else
void getstarttime(void) {
#ifdef HAVE_CLOCK_GETTIME
 struct timespec spec;
	if(!clock_gettime( CLOCK_PROCESS_CPUTIME_ID ,&spec)) { /*correlates maybe a little bit with how much time everything takes */
		startuptime= spec.tv_sec*1000000000ULL+spec.tv_nsec;
		if(startuptime==0)
			startuptime=8000001;

		}
#else
			startuptime=8000001;
#endif

#endif
showmarkswhole=0x130000000000/startuptime;
	}
/*
QueryProcessCycleTime(
  _In_  HANDLE   ProcessHandle,
  _Out_ PULONG64 CycleTime
);

*/
