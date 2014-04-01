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
#include <common/Debug.hpp>
#include <json/JsonState.h>
#include <common/SimpleAllocator.hpp>

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  CLASS  **********************/
template <class T>
class ArrayIterator
{
	public:
		ArrayIterator(T * data = NULL): data(data){};
		ArrayIterator<T>& operator++() {++data;return *this;}
		ArrayIterator<T> operator++(int) {ArrayIterator<T> tmp(*this); operator++(); return tmp;}
		bool operator==(const ArrayIterator<T>& ref) const {return data==ref.data;}
		bool operator!=(const ArrayIterator<T>& ref) const {return data!=ref.data;}
		T & operator*() {return *data;}
	private:
		T * data;
};

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
		typedef ArrayIterator<const T> const_iterator;
		typedef ArrayIterator<T> iterator;
	public:
		Array(int growCnt = 1,int maxExpGrow = (128*1024),bool defaultAlloc = false);
		Array(const Array & orig);
		~Array(void);
		T & push_back(const T & value);
		T & operator[] (int id);
		const T & operator[] (int id) const;
		void pop(void);
		size_t size(void) const {return activSize;};
		int getSize(void) const;
		int getBufferSize(void) const;
		const T * getBuffer(void) const;
		void set(const Array<T> & orig);
		iterator begin() {return iterator(buffer);};
		const_iterator begin() const {return const_iterator(buffer);};
		iterator end() {return iterator(buffer+activSize);};
		const_iterator end() const {return const_iterator(buffer+activSize);};
		Array & operator=(const Array & orig);
	public:
		template <class U> friend void convertToJson(htopml::JsonState & json, const Array<U> & value);
	private:
		void setSize(size_t size);
		void updateSize(ssize_t delta);
		void init(void);
	private:
		/** Pointer to the buffer **/
		T * buffer;
		/** Size of the buffer **/
		int bufferSize;
		/** Number of activ elements into the buffer. **/
		int activSize;
		/** Initial size when adding the first element. **/
		int initialSize;
		/** Do not use exponential grow behind this limit, then use it for linear growing.**/
		int maxExpGrow;
};

/*******************  FUNCTION  *********************/
template <class T>
Array<T>::Array(const Array<T>& orig)
{
	//params
	this->buffer = NULL;
	this->activSize = 0;
	this->bufferSize = 0;
	this->initialSize = orig.initialSize;
	this->maxExpGrow = orig.maxExpGrow;
	
	//copy
	this->set(orig);
}

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
	this->activSize = 0;
	this->bufferSize = 0;
	this->initialSize = initialSize;
	this->maxExpGrow = maxExpGrow;
	
	//get memory
	if (defaultAlloc)
	{
		setSize(1);
		setSize(0);
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
		MATT_FREE(buffer);
	buffer = NULL;
	activSize = 0;
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
	
	return activSize;
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
	assert(id >= 0 && id < activSize);
	if (id >= 0 && id < activSize)
		return buffer[id];
	else
		return *(T*)NULL;
}

/*******************  FUNCTION  *********************/
template <class T>
const T& Array<T>::operator[](int id) const
{
	assert(id >= 0 && id < activSize);
	if (id >= 0 && id < activSize)
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
	T * res = &buffer[activSize - 1];
	*res = value;
	return *res;
}

/*******************  FUNCTION  *********************/
template <class T>
void Array<T>::setSize(size_t size)
{
	this->activSize = size;
	if (activSize > bufferSize)
	{
		if (bufferSize == 0)
			bufferSize += initialSize;
		else if (bufferSize >= maxExpGrow)
			bufferSize += maxExpGrow;
		else
			bufferSize *= 2;
		
		buffer = (T*)MATT_REALLOC(buffer,bufferSize * sizeof(T));
	}
}

/*******************  FUNCTION  *********************/
template <class T>
void Array<T>::updateSize(ssize_t delta)
{
	this->setSize(this->activSize+delta);
}

/*******************  FUNCTION  *********************/
template <class T>
void Array<T>::pop(void)
{
	assert(activSize > 0);
	this->updateSize(-1);
}

/*******************  FUNCTION  *********************/
template <class T>
void Array<T>::set(const Array< T >& orig)
{
	//copy size and allocate memory if needed
	this->setSize(orig.activSize);
	
	//copy content
	for (int i =  0 ; i < orig.activSize ; i++)
		this->buffer[i] = orig.buffer[i];
}

/*******************  FUNCTION  *********************/
template <class T>
void convertToJson(htopml::JsonState& json, const Array< T >& value)
{
	json.printArray(value.buffer,value.activSize);
}

/*******************  FUNCTION  *********************/
template <class T>
Array<T> & Array<T>::operator=(const Array<T>& orig)
{
	this->set(orig);
	return *this;
}

}

#endif //MATT_ARRAY_H
