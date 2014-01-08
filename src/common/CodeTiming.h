#ifndef CODE_TIMING_H
#define CODE_TIMING_H

#include <cycle.h>
#include <string>
#include <cstdlib>

#define CODE_TIMING(name,code) do { static CodeTiming __code_timing_local__(name);  __code_timing_local__.start();do {code;} while(0);__code_timing_local__.end(); } while(0)

class CodeTiming
{
	public:
		CodeTiming(const char * name);
		~CodeTiming(void);
		void start(void);
		void end(void);
	private:
		void printValue(double value, const char * unit = "");
	private:
		std::string name;
		ticks sum;
		ticks min;
		ticks max;
		ticks lastStart;
		size_t count;
		ticks globalStart;
};

#endif //CODE_TIMING_H
