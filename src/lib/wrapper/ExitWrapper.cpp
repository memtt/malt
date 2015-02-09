#include <hooks/ExitHooks.hpp>
#include <cstdio>

/*******************  FUNCTION  *********************/
void sigKillHandler(int s)
{
	fprintf(stderr,"MATT: Capture signal KILL, dump profile and exit.");
	exit(1);
}

/*******************  FUNCTION  *********************/
void libdestructor(void) __attribute__((destructor (101)));
void libdestructor(void)
{
	MATT::ExitHooks * hook = MATT::exitHookInit();
	hook->onExit();
}