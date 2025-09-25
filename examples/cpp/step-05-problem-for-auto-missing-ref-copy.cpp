/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : examples/cpp/step-05-problem-for-auto-missing-ref-copy.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <list>

/**********************************************************/
class Data
{
	public:
		Data(size_t mem_size) {
			this->mem = new char[mem_size];
			this->initData();
		};

		Data(const Data & orig)
		{
			this->mem = new char[orig.elements];
			memcpy(this->mem, orig.mem, orig.elements);
		}

		~Data(void)
		{
			delete [] this->mem;
		}

		size_t sumAll(void)
		{
			size_t sum = 0;
			for (size_t i = 0 ; i < elements ; i++)
				sum += this->mem[i];
			return sum;
		}

	private:
		void initData(void)
		{
			for (size_t i = 0 ; i < elements ; i++)
				this->mem[i] = i;
		}

	private:
		char * mem {NULL};
		size_t elements{NULL};
};

/**********************************************************/
void function_doing_things_with_memory(void)
{
	//var
	const size_t nb = 1000;
	const size_t repeat_sum = 1000;
	std::list<Data> dataList;

	//fill
	for (size_t i = 0 ; i < nb ; i++)
		dataList.emplace_back(1024);

	//sum all
	size_t tot_sum = 0;
	for (size_t iteration = 0 ; iteration < repeat_sum ; iteration++)
		for (auto it : dataList)
			tot_sum += it.sumAll();
}

/**********************************************************/
int main(void)
{
	function_doing_things_with_memory();
	return EXIT_SUCCESS;
}
