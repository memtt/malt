/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.0-dev
             DATE     : 08/2019
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_STACK_SIZE_TRACKER_H
#define MALT_STACK_SIZE_TRACKER_H

/********************  HEADERS  *********************/
#include <common/Array.hpp>
#include <json/JsonState.h>

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*********************  CLASS  **********************/
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
		Array<unsigned long> stack;
		unsigned long cur;
		unsigned long base;
		unsigned long mapLower;
		unsigned long mapUpper;
};

}

#endif //MALT_STACK_SIZE_TRACKER_H
