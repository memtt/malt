/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <string>
#include <analysis/ProcessLevelAnalysis.hpp>
#include <core/Options.hpp>
#include <portability/OS.hpp>
#include <sys/mman.h>

/***************** USING NAMESPACE ******************/
using namespace MATT;

/*******************  FUNCTION  *********************/
TEST(ProcessLevelAnalysis,constructor)
{
	//setup
	getOptions().stackMode = "backtrace";
	ProcessLevelAnalysis pana;
}

/*******************  FUNCTION  *********************/
TEST(ProcessLevelAnalysis,initEnterExit)
{
	//setup
	getOptions().stackMode = "tree-enter-exit";
	ProcessLevelAnalysis pana;
}

/*******************  FUNCTION  *********************/
TEST(ProcessLevelAnalysis,initEnterExitMap)
{
	//setup
	getOptions().stackMode = "map-enter-exit";
	ProcessLevelAnalysis pana;
}

/*******************  FUNCTION  *********************/
TEST(ProcessLevelAnalysis,onAllocEvents)
{
	//setup
	getOptions().stackMode = "backtrace";
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
	pana.onMalloc(info,ptr,16);
	pana.onCalloc(info,ptr,1,16);
	pana.onMemalign(info,ptr,8,16);
	pana.onPosixMemalign(info,1,&ptr,8,16);
	pana.onPvalloc(info,ptr,16);
	pana.onValloc(info,ptr,16);
	pana.onRealloc(info,ptr,ptr,16);
	pana.onAlignedAlloc(info,ptr,8,16);
	EXPECT_FALSE(pana.mallocCallEnterExit());
	
	//enter-exit
	pana.onMallocEnterFunction(info,(void*)0xAAAA,(void*)0xBBBBB);
	pana.onMallocExitFunction(info,(void*)0xAAAA,(void*)0xBBBBB);
	pana.onEnterFunction((void*)0xAAAA,(void*)0xBBBB);
	pana.onExitFunction((void*)0xAAAA,(void*)0xBBBB);
	
	//mmap
	MmapHooksInfos minfo;
	pana.onMmap(minfo,ptr,ptr,16,PROT_READ|PROT_WRITE,MAP_ANON,0,0);
	pana.onMmapEnterFunction(minfo);
	pana.onMmapExitFunction(minfo);
	pana.onMunmap(minfo,0,ptr,16);
	pana.onMremap(minfo,ptr,ptr,16,16,0);
	EXPECT_FALSE(pana.mmapCallEnterExit());
	
	//cleanup
	free(ptr);
}

/*******************  FUNCTION  *********************/
TEST(ProcessLevelAnalysis,onExit)
{
	//setup
	getOptions().stackMode = "backtrace";
	ProcessLevelAnalysis pana;
	pana.onExit();
}

/*******************  FUNCTION  *********************/
TEST(ProcessLevelAnalysis,onReallocEvents)
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
TEST(ProcessLevelAnalysis,json)
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

/*******************  FUNCTION  *********************/
TEST(ProcessLevelAnalysis,getNewThreadLevelAnalysis)
{
	ProcessLevelAnalysis pana;
	ThreadLevelAnalysis * tana = pana.getNewThreadLevelAnalysis();
	EXPECT_NE((ThreadLevelAnalysis*)NULL,tana);
}

/*******************  FUNCTION  *********************/
TEST(ProcessLevelAnalysis,getNewThreadLevelAnalysisReuse)
{
	ProcessLevelAnalysis pana;
	ThreadLevelAnalysis * tana1 = pana.getNewThreadLevelAnalysis();
	tana1->setInUse(false);
	ThreadLevelAnalysis * tana2 = pana.getNewThreadLevelAnalysis();
	EXPECT_EQ(tana1,tana2);
}

/*******************  FUNCTION  *********************/

/*******************  FUNCTION  *********************/
TEST(ProcessLevelAnalysis,convertToJson)
{
	ProcessLevelAnalysis pana;
	std::stringstream out;
	htopml::convertToJson(out,pana);
	std::string ref = OS::loadTextFile(CURRENT_SOURCE_DIR "/process-data-ref-1.json");
	EXPECT_EQ(ref,out.str());
}