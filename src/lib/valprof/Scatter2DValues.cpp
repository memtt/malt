/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//std
#include <cstdlib>
#include <cmath>
//from fftw
#include <cycle.h>
//extern deps
#include <json/JsonState.h>
//internal
#include <common/Debug.hpp>
//current
#include "Scatter2DValues.hpp"

/********************  NAMESPACE  *******************/
namespace MALT
{

/*******************  FUNCTION  *********************/
Scatter2DValues::Scatter2DValues(size_t sizeX, size_t sizeY, bool logX, bool logY)
{
	//X
	this->xAxis.size = sizeX;
	this->xAxis.max = sizeX;
	this->xAxis.log = logX;
	
	//Y
	this->yAxis.size = sizeY;
	this->yAxis.max = sizeY;
	this->yAxis.log = logY;
	
	//allocate
	this->values = new size_t[sizeX*sizeY];
	this->oldValues = new size_t[sizeX*sizeY];
	
	//init
	memset(this->values,0,sizeof(size_t)*sizeX*sizeY);
	memset(this->oldValues,0,sizeof(size_t)*sizeX*sizeY);
}

/*******************  FUNCTION  *********************/
Scatter2DValues::~Scatter2DValues(void)
{
	delete [] values;
	delete [] oldValues;
}

/*******************  FUNCTION  *********************/
bool Scatter2DValues::needResize(uint64_t x, uint64_t y)
{
	return (x >= xAxis.max || y >= yAxis.max);
}

/*******************  FUNCTION  *********************/
void Scatter2DValues::incr(size_t* values, uint64_t x, uint64_t y, size_t inc)
{
	x = getIndex(x,xAxis);
	y = getIndex(y,yAxis);
	values[y * xAxis.size + x]+=inc;
}

/*******************  FUNCTION  *********************/
void Scatter2DValues::push(uint64_t x, uint64_t y)
{
	//fix 0
	if (x == 0 && xAxis.log)
		x = 1;
	if (y == 0 && yAxis.log)
		y = 1;
	
	//resize
	if (needResize(x,y))
		resize(x,y);
	
	//inc
	incr(values,x,y,1);
}

/*******************  FUNCTION  *********************/
size_t Scatter2DValues::getIndex(size_t value, const Scatter2DValuesAxis& axis) const
{
	if (axis.log)
		return (size_t) (double)(axis.size * log2(value) / log2(axis.max));
	else
		return value / (axis.max / axis.size);
}

/*******************  FUNCTION  *********************/
size_t Scatter2DValues::getValue(size_t index, const Scatter2DValuesAxis& axis) const
{
	if (axis.log)
		return pow(2,log2(axis.max) *(double)index/(double)axis.size);
	else
		return index * (axis.max / axis.size);
}

/*******************  FUNCTION  *********************/
void Scatter2DValues::resize(uint64_t x, uint64_t y)
{
	//reset
	memset(this->oldValues,0,sizeof(size_t)*xAxis.size*yAxis.size);
	
		//swap values
	size_t * tmp = values;
	values = oldValues;
	oldValues = tmp;
	
	//new
	Scatter2DValuesAxis oldXAxis = xAxis;
	Scatter2DValuesAxis oldYAxis = yAxis;
	
	//find max
	while (xAxis.max <= x)
		xAxis.max *= 2;
	while (yAxis.max <= y)
		yAxis.max *= 2;
	
	//fill
	for (size_t j = 0 ; j < oldYAxis.size ; j++)
	{
		for (size_t i = 0 ; i < oldXAxis.size ; i++)
		{
			//axtract
			size_t vx = getValue(i,oldXAxis);
			size_t vy = getValue(j,oldYAxis);
			size_t v = oldValues[j * oldXAxis.size + i];
			
			//fill again
			incr(values,vx,vy,v);
		}
	}
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const Scatter2DValuesAxis& value)
{
	json.openStruct();
		json.printField("max",value.max);
		json.printField("size",value.size);
		json.printField("log",value.log);
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const Scatter2DValues& value)
{
	json.openStruct();
		//infos
		json.openFieldStruct("infos");
			json.printField("xAxis",value.xAxis);
			json.printField("yAxis",value.yAxis);
		json.closeFieldStruct("infos");
		
		//points
		json.openFieldArray("points");
			for (uint64_t y = 0 ; y < value.yAxis.size ; y++)
			{
				for (uint64_t x = 0 ; x < value.xAxis.size ; x++)
				{
					size_t v = value.values[y * value.yAxis.size + x];
					if (v != 0)
					{
						json.printListSeparator();
						json.openStruct();
							json.printField("x",value.getValue(x,value.xAxis));
							json.printField("y",value.getValue(y,value.yAxis));
							json.printField("v",v);
						json.closeStruct();
					}
				}
			}
		json.closeFieldArray("points");
	json.closeStruct();
}

}
