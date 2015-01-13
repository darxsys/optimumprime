#ifndef __TIMERH__
#define __TIMERH__

#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct timeval Timeval;

extern void timerStart(Timeval* timer);

extern long long timerStop(Timeval* timer);

extern double timerStopSecs(Timeval* timer);

extern void timerPrint(char message[], long long usec);

extern void timerPrintSecs(char message[], double sec);

#ifdef __cplusplus
}
#endif
#endif  // __TIMERH__
