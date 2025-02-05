/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.5
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/core/StackSizeTracker.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2021 - 2024
***********************************************************/

#ifndef MALT_STACK_SIZE_TRACKER_H
#define MALT_STACK_SIZE_TRACKER_H

/**********************************************************/
#include <common/Array.hpp>
#include <json/JsonState.h>

/**********************************************************/
namespace MALT
{

/**********************************************************/
/**
 * @brief Class used to track the size of the stack of the current thread.
**/
class StackSizeTracker
{
	public:
		StackSizeTracker(void);
		void enter(void);
		void exit(void);
		unsigned long getSize(void) const;
		unsigned long getTotalSize(void) const;
		StackSizeTracker & operator=(const StackSizeTracker & orig);
	public:
		friend void convertToJson(htopml::JsonState & json, const StackSizeTracker & value);
	private:
		void loadMapping(void);
	private:
		/** Keep track of the stack size elements **/
		Array<unsigned long> stack;
		/** Current address of the stack pointer. **/
		unsigned long cur;
		/** 
		 * Base address of the stack to be retained from the current address in
		 * order to get the size.
		**/
		unsigned long base;
		/** Real base address of the stack extracted from /proc/self/maps **/
		unsigned long mapLower;
		/** Real upper address of the stack extracted from /proc/self/maps **/
		unsigned long mapUpper;
};

}

#endif //MALT_STACK_SIZE_TRACKER_H
