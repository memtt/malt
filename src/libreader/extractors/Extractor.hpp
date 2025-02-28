/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/Backtrace.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_READER_EXTRACTOR_HPP
#define MALT_READER_EXTRACTOR_HPP

/**********************************************************/
#include <functional>
#include "format/MaltProfile.hpp"

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
struct InstructionInfosStrRef
{
	const std::string * file;
	const std::string * binary;
	const std::string * function;
	ssize_t line;
};

/**********************************************************/
struct FlatProfileValue
{
	MALTFormat::StackInfos own;
	MALTFormat::StackInfos total;
	size_t line{0};
};

/**********************************************************/
typedef std::function<std::string(const InstructionInfosStrRef & /*location*/, const MALTFormat::StackInfos & /*infos*/)> LocaltionMappingFunc;
typedef std::function<std::string(const InstructionInfosStrRef & /*location*/, const MALTFormat::StackInfos & /*infos*/)> LocaltionFilterFunc;
typedef std::map<std::string, FlatProfileValue> FlatProfile;

/**********************************************************/
class Extractor
{
	public:
		Extractor(const MALTFormat::MaltProfile & profile);
		~Extractor(void);
		FlatProfile getFlatProfile(const LocaltionMappingFunc & mapping,const LocaltionFilterFunc & filter);
	private:
		const MALTFormat::MaltProfile & profile;
};

}

#endif //MALT_READER_EXTRACTOR_HPP
