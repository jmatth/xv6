#ifndef _SIGNAL_
#define _SIGNAL_ 1
#define NSIGS 1

// map trap numbers to signals.
#define SIGSEGV 0

extern void __tramp(int);
#endif
