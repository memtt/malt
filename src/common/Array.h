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
/**
 * Provide a short implementation of dynamic arrays which can grow
 * exponentially until a certain limit then grow linearly.
 * 
 * @b CAUTION it will use realloc so please, do not use objects
 * witch cannot be moved. It's mostly built for basic types.
 * 
 * @brief A short dynamic array implementation.
**/
template <class T>
class Array
{
	public:
		Array(int growCnt,int maxExpGrow,bool defaultAlloc);
		~Array(void);
		T & push_back(const T & value);
		T & operator[] (int id);
		void pop(void);
		int getSize(void) const;
		int getBufferSize(void) const;
		const T * getBuffer(void) const;
		void set(const Array<T> & orig);
	private:
		void setSize(size_t size);
		void updateSize(ssize_t delta);
	private:
		//Copy isn't implemented yet
		Array(const Array & orig);
		Array & operator=(const Array & orig);
	private:
		/** Pointer to the buffer **/
		T * buffer;
		/** Size of the buffer **/
		int bufferSize;
		/** Number of activ elements into the buffer. **/
		int size;
		/** Initial size when adding the first element. **/
		int initialSize;
		/** Do not use exponential grow behind this limit, then use it for linear growing.**/
		int maxExpGrow;
};

/*******************  FUNCTION  *********************/
/**
 * Array constructor. By default it does not allocate memory.
 * @param initialSize Define the memory to allocate on first add.
 * @param maxExpGrow Define exponential growing limit.
 * @param defaultAlloc If true, initially allocate the buffer memory instead of using a lazy approch.
**/
template <class T>
Array<T>::Array(int initialSize, int maxExpGrow,bool defaultAlloc)
{
	//params
	this->buffer = NULL;
	this->size = 0;
	this->bufferSize = 0;
	this->initialSize = initialSize;
	this->maxExpGrow = maxExpGrow;
	
	//get memory
	if (defaultAlloc)
	{
		updateSize(1);
		updateSize(-1);
	}
}

/*******************  FUNCTION  *********************/
/**
 * Destructor of array.
**/
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
/**
 * @return Return address of current buffer. Caution, it may change when adding
 * content to the array.
**/
template <class T>
const T* Array<T>::getBuffer(void) const
{
	return buffer;
}

/*******************  FUNCTION  *********************/
/**
 * @return Number of active elements in array.
**/
template <class T>
int Array<T>::getSize(void) const
{
	
	return size;
}

/*******************  FUNCTION  *********************/
/**
 * @return Current size (elements) of the internal buffer.
**/
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
	//T * res = new (buffer+size-1) T(value);
	T * res = &buffer[size - 1];
	*res = value;
	return *res;
}

/*******************  FUNCTION  *********************/
template <class T>
void Array<T>::setSize(size_t size)
{
	if (size > bufferSize)
	{
		if (bufferSize == 0)
			bufferSize += initialSize;
		else if (bufferSize >= maxExpGrow)
			bufferSize += maxExpGrow;
		else
			bufferSize *= 2;
		
		buffer = (T*)realloc(buffer,bufferSize * sizeof(T));
	}
}

/*******************  FUNCTION  *********************/
template <class T>
void Array<T>::updateSize(ssize_t delta)
{
	size += delta;
	this->setSize(size);
}

/*******************  FUNCTION  *********************/
template <class T>
void Array<T>::pop(void)
{
	this->updateSize(-1);
}

/*******************  FUNCTION  *********************/
template <class T>
void Array<T>::set(const Array< T >& orig)
{
	//copy size and allocate memory if needed
	this->setSize(orig.size);
	
	//copy content
	for (int i =  0 ; i < orig.size ; i++)
		this->buffer[i] = orig.buffer[i];
}

}

#endif //MATT_ARRAY_H
