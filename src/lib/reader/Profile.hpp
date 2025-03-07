/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/Backtrace.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_READER_PROFILE_HPP
#define MALT_READER_PROFILE_HPP

/**********************************************************/
#include "format/MaltProfile.hpp"
#include "extractors/Extractor.hpp"

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
class Profile
{
	public:
		Profile(const std::string & fname, bool loadProgressBar = false);
		~Profile(void);
		const Extractor & getExtractor(void) const;
	private:
		MALTFormat::MaltProfile profile;
		Extractor * extractor{nullptr};
};

}

#endif //MALT_READER_PROFILE_HPP
