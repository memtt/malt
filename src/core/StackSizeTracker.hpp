/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_STACK_SIZE_TRACKER_H
#define MATT_STACK_SIZE_TRACKER_H

/********************  HEADERS  *********************/
#include <common/Array.h>
#include <json/JsonState.h>

/*******************  NAMESPACE  ********************/
namespace MATT
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
		unsigned long getTLS(void) const;
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

#endif //MATT_STACK_SIZE_TRACKER_H
