/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//gest
#include <gtest/gtest.h>
//internal
#include <analysis/ProcessLevelAnalysis.hpp>
#include <analysis/ThreadLevelAnalysis.hpp>

/***************** USING NAMESPACE ******************/
using namespace MATT;

/*******************  FUNCTION  *********************/
TEST(ThreadLevelAnalysis,constructor)
{
	//setup
	ProcessLevelAnalysis pana;
	ThreadLevelAnalysis tana(&pana);
}

/*******************  FUNCTION  *********************/
TEST(ThreadLevelAnalysis,inUse)
{
	//setup
	ProcessLevelAnalysis pana;
	ThreadLevelAnalysis tana(&pana);
	
	EXPECT_FALSE(tana.isInUse());
	tana.setInUse(true);
	EXPECT_TRUE(tana.isInUse());
	tana.setInUse(false);
	EXPECT_FALSE(tana.isInUse());
}

/*******************  FUNCTION  *********************/
TEST(ThreadLevelAnalysis,mallocCallEnterExit)
{
	//setup
	ProcessLevelAnalysis pana;
	ThreadLevelAnalysis tana(&pana);
	
	EXPECT_FALSE(tana.mallocCallEnterExit());
}

/*******************  FUNCTION  *********************/
TEST(ThreadLevelAnalysis,onAllocEvents)
{
	//setup
	ProcessLevelAnalysis pana;
	ThreadLevelAnalysis tana(&pana);
	
	//required as it init the stack handlers
	tana.setInUse(true);
	
	//setup stack info
	MallocHooksInfos info;
	tana.setupStack(info,2);
	
	//get a pointer
	void * ptr = malloc(16);
	
	//test on actions
	tana.onMalloc(info,ptr,16);
	tana.onCalloc(info,ptr,1,16);
	tana.onMemalign(info,ptr,8,16);
	tana.onPosixMemalign(info,1,&ptr,8,16);
	tana.onPvalloc(info,ptr,16);
	tana.onValloc(info,ptr,16);
	tana.onRealloc(info,ptr,ptr,16);
	tana.onAlignedAlloc(info,ptr,8,16);
	
	//enter-exit
	tana.onMallocEnterFunction(info,(void*)0xAAAA,(void*)0xBBBBB);
	tana.onMallocExitFunction(info,(void*)0xAAAA,(void*)0xBBBBB);
	tana.onEnterFunction((void*)0xAAAA,(void*)0xBBBB);
	tana.onExitFunction((void*)0xAAAA,(void*)0xBBBB);
	
	//cleanup
	free(ptr);
}

/*******************  FUNCTION  *********************/
TEST(ThreadLevelAnalysis,onReallocEvents)
{
	//setup
	ProcessLevelAnalysis pana;
	ThreadLevelAnalysis tana(&pana);
	
	//required as it init the stack handlers
	tana.setInUse(true);
	
	//setup stack info
	MallocHooksInfos info;
	tana.setupStack(info,2);
	
	//get a pointer
	void * ptr = malloc(16);
	tana.onMalloc(info,ptr,16);
	tana.onPreRealloc(info,ptr,32);
	void * ptr2 = realloc(ptr,32);
	tana.onRealloc(info,ptr2,ptr,32);
	tana.onPreFree(info,ptr);
	free(ptr2);
	tana.onFree(info,ptr);
}

/*******************  FUNCTION  *********************/
TEST(ThreadLevelAnalysis,json)
{
	//setup
	ProcessLevelAnalysis pana;
	ThreadLevelAnalysis tana(&pana);
	
	//required as it init the stack handlers
	tana.setInUse(true);
	
	//setup stack info
	MallocHooksInfos info;
	tana.setupStack(info,2);
	
	//get a pointer
	void * ptr = malloc(16);
	
	//test on actions
	tana.onMalloc(info,ptr,16);
	tana.onPreFree(info,ptr);
	free(ptr);
	tana.onFree(info,ptr);
	
	//render json
	std::stringstream out;
	htopml::convertToJson(out,tana);
	EXPECT_NE("",out.str());
}
