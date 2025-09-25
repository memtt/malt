/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 07/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/loader/Prepare.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_READER_PREPARE_HPP
#define MALT_READER_PREPARE_HPP

/**********************************************************/
#include "format/MaltProfile.hpp"

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
void prepareProfile(MALTFormat::MaltProfile & profile);

}

#endif //MALT_READER_PREPARE_HPP
