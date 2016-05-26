/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <common/Debug.hpp>
#include <common/Helpers.hpp>
#include "UserSegmentTracker.hpp"

/********************  NAMESPACE  *******************/
namespace MATT
{

/*******************  FUNCTION  *********************/
UserSegmentTracker::UserSegmentTracker ( bool threadSafe )
{
	this->threadSafe = threadSafe;
}

/*******************  FUNCTION  *********************/
void UserSegmentTracker::registerChunk ( void* ptr, size_t size, ticks birth, MATT::StackId stackId, int allocThreadId, MATT::StackTreeDataHandler dataHandler )
{
	UserSegment & chunk = map[ptr];
	chunk.size = size;
	chunk.birth = birth;
	chunk.stackId = stackId;
	chunk.allocThreadId =allocThreadId;
	chunk.dataHandler = dataHandler;
}

/*******************  FUNCTION  *********************/
void convertToJson ( htopml::JsonState& json, const UserSegmentTracker& value )
{
	char buffer[256];
	json.openStruct();
		foreachConst(UserSegmentInfoMap,value.map,it)
		{
			sprintf(buffer,"%p",it->first);
			json.printField(buffer,it->second);
		}
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
void convertToJson ( htopml::JsonState& json, const UserSegment& value)
{
	json.openStruct();
		json.printField("threadID",value.allocThreadId);
		json.printField("birth",value.birth);
		json.printField("stackId",value.stackId);
		json.printField("size",value.size);
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
void convertToJson ( htopml::JsonState& json, const LeakSummary& value )
{
	json.openStruct();
		json.printField("count",value.count);
		json.printField("mem",value.mem);
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
void UserSegmentTracker::buildLeakSummary ( LeakSummaryMap& leakSummaryMap )
{
	for (UserSegmentInfoMap::const_iterator it = this->map.begin() ; it != this->map.end() ; ++it)
	{
		LeakSummaryMap::iterator it2 = leakSummaryMap.find(it->second.stackId);
		if (it2 == leakSummaryMap.end())
		{
			LeakSummary summary;
			summary.count = 1;
			summary.mem = it->second.size;
			leakSummaryMap[it->second.stackId] = summary;
		} else {
			leakSummaryMap[it->second.stackId].count++;
			leakSummaryMap[it->second.stackId].mem += it->second.size;
		}
	}
}

/*******************  FUNCTION  *********************/
UserSegment UserSegmentTracker::unregister ( Clock & clock,void* ptr )
{
	UserSegmentInfoMap::iterator it = map.find(ptr);
	if (it == map.end())
	{
		UserSegment ret;
		ret.size = -1UL;
		return ret;
	} else {
		UserSegment ret = it->second;
		map.erase(it);
		return ret;
	}
}

}

