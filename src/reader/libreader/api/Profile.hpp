/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 07/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/api/Profile.hpp
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
		virtual ~Profile(void);
		const Extractor & getExtractor(void) const;
	protected:
		MALTFormat::MaltProfile profile;
		Extractor * extractor{nullptr};
};

}

#endif //MALT_READER_PROFILE_HPP
