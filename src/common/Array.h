/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_ARRAY_H
#define MATT_ARRAY_H

/********************  HEADERS  *********************/
#include <cstdlib>
#include <cassert>

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  CLASS  **********************/
template <class T>
class Array
{
	public:
		Array(int growCnt,int maxExpGrow,bool defaultAlloc);
		~Array(void);
		T & push_back(const T & value);
		T & operator[] (int id);
		int getSize(void) const;
		int getBufferSize(void) const;
		const T * getBuffer(void) const;
	private:
		void updateSize(ssize_t delta);
	private:
		T * buffer;
		int bufferSize;
		int size;
		int growCnt;
		int maxExpGrow;
};

/*******************  FUNCTION  *********************/
template <class T>
Array<T>::Array(int growCnt, int maxExpGrow,bool defaultAlloc)
{
	//params
	this->buffer = NULL;
	this->size = 0;
	this->bufferSize = 0;
	this->growCnt = growCnt;
	this->maxExpGrow = maxExpGrow;
	
	if (defaultAlloc)
	{
		updateSize(1);
		updateSize(-1);
	}
}

/*******************  FUNCTION  *********************/
template <class T>
Array<T>::~Array(void)
{
	if (buffer != NULL)
		free(buffer);
	buffer = NULL;
	size = 0;
	bufferSize = 0;
}

/*******************  FUNCTION  *********************/
template <class T>
const T* Array<T>::getBuffer(void) const
{
	return buffer;
}

/*******************  FUNCTION  *********************/
template <class T>
int Array<T>::getSize(void) const
{
	return size;
}

/*******************  FUNCTION  *********************/
template <class T>
int Array<T>::getBufferSize(void) const
{
	return bufferSize;
}

/*******************  FUNCTION  *********************/
template <class T>
T& Array<T>::operator[](int id)
{
	assert(id >= 0 && id < size);
	if (id >= 0 && id < size)
		return buffer[id];
	else
		return *(T*)NULL;
}

/*******************  FUNCTION  *********************/
template <class T>
T & Array<T>::push_back(const T& value)
{
	updateSize(1);
	T * res = new (buffer+size-1) T(value);
	return *res;
}

/*******************  FUNCTION  *********************/
template <class T>
void Array<T>::updateSize(ssize_t delta)
{
	size += delta;
	if (size > bufferSize)
	{
		if (bufferSize == 0)
			bufferSize += growCnt;
		else if (bufferSize >= maxExpGrow)
			bufferSize += maxExpGrow;
		else
			bufferSize *= 2;
		
		buffer = (T*)realloc(buffer,bufferSize * sizeof(T));
	}
}

}

#endif //MATT_ARRAY_H
