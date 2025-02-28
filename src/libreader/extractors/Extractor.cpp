/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/Backtrace.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include "Extractor.hpp"

/**********************************************************/
using namespace MALTFormat;

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
Extractor::Extractor(const MALTFormat::MaltProfile & profile)
	:profile(profile)
{
}

/**********************************************************/
Extractor::~Extractor(void)
{
}

/**********************************************************/
FlatProfile Extractor::getFlatProfile(const LocaltionMappingFunc & mapping,const LocaltionFilterFunc & filter)
{
	//vars
	FlatProfile result;
	const StackStats & stats = this->profile.stacks.stats;

	//loop on all
	for (const auto & statEntry : stats) {
		
	}

	//get local
	/*
	//setup some local vars
	///var stats = this.data.stacks.stats;
	///var res = new Object();
	////var callers = "total";
	var cur = null;
	///if (total == false)
	///	callers = "childs";

	for(var i in stats)
	{
		//extract some short refs
		var statsEntry = stats[i];
		var detailedStack = statsEntry.detailedStack;
		var infos = statsEntry.infos;
		var stack = statsEntry.stack;

		//skip C++ operators
		var skip = 0;
		while (skip < detailedStack.length && isAllocFunction(detailedStack[skip].function)) skip++;
		if (skip >= detailedStack.length)
		{
			console.log("Warning : get call stacks with only allocation function ??? : "+JSON.stringify(detailedStack) +" -> "+JSON.stringify(statsEntry));
			continue;
		}

		//update internal values
		cur = detailedStack[skip];
		if (accept == true || accept(cur,infos) == true)
			mergeStackInfo(res,cur,stack[skip],"own",infos,mapping,fields);

		//childs
		var done = new Object;
		for (var j in stack)
		{
			//skip firsts for 'own' mode, otherwise keep them
			if (total == false && j <= skip)
				continue;

			//extract some quick refs
			cur = detailedStack[j];
			var key = mapping(cur);
			var filter = (accept == true || accept(cur,infos) == true);
			if (filter && done[key] == undefined && !isAllocFunction(cur.function))
			{
				done[key] = true;
				mergeStackInfo(res,cur,stack[j],callers,infos,mapping,fields);
			}
		}
	}

	//convert to simple list
	var finalRes = new Array();
	for (var i in res)
		finalRes.push(res[i]);

	//ok return
	return finalRes;
	*/
}

}
