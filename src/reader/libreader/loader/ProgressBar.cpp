/***********************************************************
*    PROJECT  : Gray Scott Simulation
*    VERSION  : 0.0.0
*    DATE     : 11/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/display/ProgressBar.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2024
***********************************************************/
#include <cassert>
#include <cstdio>
#include <sstream>
#include <sys/ioctl.h>
#include <cstdio>
#include "ProgressBar.hpp"

/**********************************************************/
using namespace MALTReader;

/**********************************************************/
ProgressBar::ProgressBar(size_t total_steps, size_t draw_width, size_t elements_computed)
{
	this->total_steps = total_steps;
	this->draw_width = draw_width;
	this->elements_computed = elements_computed;

	if (draw_width == MALT_DRAW_WIDTH_AUTO) {
		struct winsize w;
		ioctl(0, TIOCGWINSZ, &w);
		this->draw_width = w.ws_col - 40;
	}

	if(this->draw_width < 0 || this->draw_width > 500)
		this->draw_width = 40;
}

/**********************************************************/
ProgressBar::~ProgressBar(void)
{

}

/**********************************************************/
void ProgressBar::end(void) const
{
	//display
	fprintf(stderr, "\n");
	fflush(stderr);
}

/**********************************************************/
void ProgressBar::progress(size_t steps)
{
	//check
	assert(steps < this->total_steps);

	//incr
	this->current_step += steps;
	assert(this->current_step <= this->total_steps);

	//extract
	const size_t draw_width = this->draw_width;
	const size_t total_steps = this->total_steps;
	const size_t current_step = this->current_step;

	//calc
	const size_t limit = (current_step * draw_width) / total_steps;
	const size_t percent = (float)(current_step * 100) / (float)total_steps;

	//redraw
	if (last_display != percent) {
		//buffers
		std::stringstream buffer;

		//redraw
		buffer << "[";
		for (size_t i = 0 ; i < draw_width ; i++) {
			if (i < limit)
				buffer << "=";
			else if (i == limit)
				buffer << ">";
			else
				buffer << " ";
		}
		buffer << "] " << percent << " %      ";

		//display
		fprintf(stderr, "\r%s", buffer.str().c_str());
		fflush(stderr);

		//display
		last_display = percent;

		//last
		if (last_display == 100)
			this->end();
	}
}
