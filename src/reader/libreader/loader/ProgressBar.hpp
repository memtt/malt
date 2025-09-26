/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/loader/ProgressBar.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/********************* IMPORTED FROM ***********************
*    PROJECT  : Gray Scott Simulation
*    VERSION  : 0.0.0
*    DATE     : 12/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/display/ProgressBar.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2024
***********************************************************/

/**********************************************************/
#ifndef MALT_READER_PROGRESS_BAR_HPP
#define MALT_READER_PROGRESS_BAR_HPP

/**********************************************************/
#include <cstdlib>

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
#define MALT_DRAW_WIDTH_AUTO 0

/**********************************************************/
class ProgressBar
{
	public:
		ProgressBar(size_t total_steps, size_t draw_width, size_t elements_computed);
		~ProgressBar(void);
		void progress(size_t steps);
		void end() const;
	private:
		size_t total_steps{0};
		size_t current_step{0};
		size_t draw_width{0};
		size_t last_display{0};
		size_t elements_computed{0};
};

}

#endif //MALT_READER_PROGRESS_BAR_HPP
