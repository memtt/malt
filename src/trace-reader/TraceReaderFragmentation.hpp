/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.2
*    DATE     : 09/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/trace-reader/TraceReaderFragmentation.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2024
***********************************************************/

#ifndef MALT_TRACE_READER_FRAGMENTATION_HPP
#define MALT_TRACE_READER_FRAGMENTATION_HPP

/**********************************************************/
#include <map>
#include "TraceReader.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
struct FragmentationChunk
{
	AllocTracerChunk chunk;
	bool presentAtStep2;
	bool haveSharedPage;
};

/**********************************************************/
class FragmentationChunkPerCallStack
{
	public:
		FragmentationChunkPerCallStack();
		void push(size_t size,ticks lifetime);
		void print(const MALT::Stack* stack) const;
	private:
		size_t count;
		size_t sum;
		size_t min;
		size_t max;
		ticks sumLifetime;
};

/**********************************************************/
class Stack;
typedef std::map<void*,FragmentationChunk> FragmentationChunkMap;
typedef std::map<const Stack *,FragmentationChunkPerCallStack> FragmentationChunkPerCallStackMap;

/**********************************************************/
class TraceReaderFragmentation : public TraceReader
{
	public:
		TraceReaderFragmentation(ticks tStep1,ticks tStep2,bool details,Filter * filter = NULL);
		virtual void onStart(void);
		virtual void onData(MALT::AllocTracerChunk & chunk);
		virtual void onEnd(void);
	private:
		void checkForSharedPages(void);
		void printDetails(void);
		void printPerCallStack(void);
	private:
		ticks tStep1;
		ticks tStep2;
		bool details;
		FragmentationChunkMap chunks;
};

}

#endif //MALT_TRACE_READER_FRAGMENTATION_HPP
