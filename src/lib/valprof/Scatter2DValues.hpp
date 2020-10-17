/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_SCATTER_2D_VALUES_HPP
#define MALT_SCATTER_2D_VALUES_HPP

/********************  HEADERS  *********************/
//std
#include <cstdlib>
#include <stdint.h>
//extern deps
#include <json/JsonState.h>

/********************  NAMESPACE  *******************/
namespace MALT
{

/*********************  CLASS  **********************/
struct Scatter2DValuesAxis
{
	/** Number of cells on the given axis **/
	size_t size;
	/** Use log **/
	bool log;
	/** Max values **/
	uint64_t max;
};

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const Scatter2DValuesAxis & value);

/*********************  CLASS  **********************/
/**
 * This class help building scatter 2D chart, mostly used to track chunk size over time
 * and lifetime over chunk size. It discretize the 2D space and count the number of occurences
 * on each cells. In the output if provide coordinate and counter for each non empty cells.
 * It also pemit to use logarithmique scales on X and Y.
**/
class Scatter2DValues
{
	public:
		Scatter2DValues(size_t sizeX,size_t sizeY,bool logX,bool logY);
		~Scatter2DValues(void);
		void push(uint64_t x,uint64_t y);
	public:
		friend void convertToJson(htopml::JsonState& json, const Scatter2DValues & value);
	private:
		bool needResize(uint64_t x,uint64_t y);
		void resize(uint64_t x,uint64_t y);
		size_t getIndex(size_t value, const MALT::Scatter2DValuesAxis& axis) const;
		size_t getValue(size_t index, const MALT::Scatter2DValuesAxis& axis) const;
		void incr(size_t * values,uint64_t x,uint64_t y,size_t inc = 1);
	private:
		/** Parameters for x axis **/
		Scatter2DValuesAxis xAxis;
		/** Parameters for y axis **/
		Scatter2DValuesAxis yAxis;
		/** Store values in 2D array (flat representation) **/
		size_t * values;
		/** Used for resizing **/
		size_t * oldValues;
};

}

#endif //MALT_SCATTER_2D_VALUES_HPP
