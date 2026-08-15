/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 04/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/api/Profile.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
*    AUTHOR   : Sébastien Valat - 2026
***********************************************************/

#ifndef MALT_READER_PROFILE_HPP
#define MALT_READER_PROFILE_HPP

/**********************************************************/
#include "format/MaltProfile.hpp"
#include "extractors/Extractor.hpp"
#include "trace/TraceReader.hpp"
#include "libinstrum/portability/Visibility.hpp"

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
class Profile
{
	public:
		DLL_PUBLIC Profile(const std::string & fname, bool loadProgressBar = false);
		DLL_PUBLIC virtual ~Profile(void);
		DLL_PUBLIC const Extractor & getExtractor(void) const;
		DLL_PUBLIC const std::string & getFileName(void) const;
		DLL_PUBLIC const Trace & getTrace(void) const;
	protected:
		const std::string fname;
		MALTFormat::MaltProfile profile;
		Extractor * extractor{nullptr};
		TraceReader * traceReader{nullptr};
};

}

#endif //MALT_READER_PROFILE_HPP
