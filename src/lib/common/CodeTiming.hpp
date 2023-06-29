/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_CODE_TIMING_H
#define MALT_CODE_TIMING_H

/********************  HEADERS  *********************/
//standards
#include <string>
#include <cstdlib>
#include <ostream>
#include <iostream>
//from fftw (copied into extern-deps)
#include <cycle.h>
//internal
#include <config.h>

/********************  MACROS  **********************/
/**
 * A short macro to quicly profile some code lines. 
\code{.cpp}
void yourFunc(void)
{
	CODE_TIMING( "rand" , int r = rand() );
}
\endcode
**/
#ifdef MALT_ENABLE_CODE_TIMING
	#define CODE_TIMING(name,code) \
		do {  \
			static MALT::CodeTiming __code_timing_local##__LINE__##__(name);   \
			ticks __code_timing_start##__line__##__ = __code_timing_local##__LINE__##__.start(); \
			do {code;} while(0); \
			__code_timing_local##__LINE__##__.end(__code_timing_start##__line__##__);  \
		} while(0)
#else
	#define CODE_TIMING(name,code) do{code;}while(0)
#endif


/********************  MACROS  **********************/
#ifdef MALT_ENABLE_CODE_TIMING
	/**
	 * Provide a short way to measure code timings by using a star/stop representation.
	 * @b CAUTION it use a local symbol so you cannot use more than one per scope. One might change this macro 
	 * and use the name parameter to build the symbol name.
	 * @param name Define the name to display at exit time.
	**/
	#define CODE_TIMING_FUNC_START(name) static MALT::CodeTiming __code_timing_local_func__(name); ticks __code_timing_start_func__ = __code_timing_local_func__.start()
	/**
	 * Provide a short way to measure code timings by using a star/stop representation.
	 * @b CAUTION it use a local symbol so you cannot use more than one per scope. One might change this macro 
	 * and use the name parameter to build the symbol name.
	 * @param name Define the name to display at exit time.
	**/
	#define CODE_TIMING_FUNC_STOP(name) __code_timing_local_func__.end(__code_timing_start_func__)
#else //MALT_ENABLE_CODE_TIMING
	#define CODE_TIMING_FUNC_START(name) do{}while(0)
	#define CODE_TIMING_FUNC_STOP(name) do{}while(0)
#endif //MALT_ENABLE_CODE_TIMING
	
	
/*******************  NAMESPACE  ********************/
namespace MALT
{

/*********************  CLASS  **********************/
/**
 * @brief Short code to profile functions.
 * 
 * Provide some basic mecanism to help optimization by measuring execution time of some codes.
 * Objects of this class may be declared static of globals to get the same lifecycle than the process.
 * The basic usage might be :
\code{.cpp}
void yourFun(void)
{
	static CodeTiming timing( "youFunc" );
	ticks t = timing.start();
	//your job
	timing.stop(t);
}
\endcode
 *
 * \b CAUTION, it didn't support recursion.
**/
class CodeTiming
{
	public:
		CodeTiming(const char * name, bool registerToGlob = false);
		~CodeTiming(void);
		ticks start(void);
		void end(ticks start, ticks end = 0);
		void finalPrint(std::ostream & out = std::cerr, bool force = false) const;
		const char * getName(void) const {return name;};
		static void printAll(std::ostream & out = std::cerr, bool force = false);
	private:
		static int compare(const void * a,const void * b);
		static void registerTimer(CodeTiming * timer);
		static void timerFinish(CodeTiming * timer);
	private:
		/** Name of the code section to profile, only used for display. **/
		const char * name;
		/** Cumulativ execution time spent in the profiled code. **/
		ticks sum;
		/** Minimum execution time of the profiled code. **/
		ticks min;
		/** Maximum execution time of the profiled code. **/
		ticks max;
		/** Number of calls. **/
		size_t count;
		/** Store the global reference time to compute global cost ratios. **/
		static ticks globalStart;
		/** Numbers of registered timers **/
		static int globalCntTimers;
		/** Number of finished timers **/
		static int globalCntTimersFinished;
		/** Store pointers to registered timers for final print. **/
		static CodeTiming * globalTimers[64];
};

/*******************  FUNCTION  *********************/
/** Start measurement of a new call to your code. **/
inline ticks CodeTiming::start(void)
{
	#ifdef MALT_ENABLE_CODE_TIMING
		return getticks();
	#else
		return 0;
	#endif
}

/*******************  FUNCTION  *********************/
/** End measurement of the current call to your code. **/
inline void CodeTiming::end(ticks start, ticks end)
{
	bool doIt = false;
	if (end > 0)
		doIt = true;
	#ifdef MALT_ENABLE_CODE_TIMING
		doIt = true;
	#endif
	if (doIt) {
		//get time
		if (end == 0)
			end = getticks();
		ticks t = end - start;

		//update min/max
		if (count == 0)
		{
			min = max = t;
		} else {
			if (t < min) min = t;
			if (t > max) max = t;
		}
		
		//update global sums
		count++;
		sum += t;
	}
}

/*******************  FUNCTION  *********************/
ticks ticksPerSecond(ticks forceValue = 0);
ticks mockableGetTicks(void);

}

#endif //MALT_CODE_TIMING_H
