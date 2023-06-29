/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_ARRAY_H
#define MALT_ARRAY_H

/********************  HEADERS  *********************/
//standard
#include <cstdlib>
#include <cassert>
//common
#include <common/Debug.hpp>
#include <common/SimpleAllocator.hpp>
//htopml json converter
#include <json/JsonState.h>

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*********************  CLASS  **********************/
/**
 * Short class to implement iterators compatible with STL semantic.
**/
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
		/** Keep track of current element. **/
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
		/** Provide iterators to keep same interface as vector. **/
		typedef ArrayIterator<const T> const_iterator;
		/** Provide iterators to keep same interface as vector. **/
		typedef ArrayIterator<T> iterator;
	public:
		Array(int growCnt = 1,int maxExpGrow = (128*1024),bool defaultAlloc = false);
		Array(const Array & orig);
		~Array(void);
		T & push_back(const T & value);
		T & operator[] (int id);
		const T & operator[] (int id) const;
		void pop(void);
		size_t size(void) const;
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
/**
 * Copy constructor.
 * @param orig Define the original array to copy on new one.
**/
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
		MALT_FREE(buffer);
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
size_t Array<T>::size(void) const
{
	return activSize;
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
/**
 * Permut usage of operator [], if will fail with NULL pointer dereference if
 * ID is invalid. In debug mode it will produce an assertion failure.
 * @param id Id of element to return.
**/
template <class T>
T& Array<T>::operator[](int id)
{
	//checks
	assumeArg(id >= 0 && id < activSize, "Invalid index (%1) to access an array of size %2 !")
		.arg(id)
		.arg(activSize)
		.end();
	
	//ok
	return buffer[id];
}

/*******************  FUNCTION  *********************/
/**
 * Permut usage of operator [], if will fail with NULL pointer dereference if
 * ID is invalid. In debug mode it will produce an assertion failure.
 * @param id Id of element to return.
**/
template <class T>
const T& Array<T>::operator[](int id) const
{
	//checks
	assumeArg(id >= 0 && id < activSize, "Invalid index (%1) to access an array of size %2 !")
		.arg(id)
		.arg(activSize)
		.end();

	//ok can return
	return buffer[id];
}

/*******************  FUNCTION  *********************/
/**
 * Insert a new element and end.
 * @param value New value to add to the array.
**/
template <class T>
T & Array<T>::push_back(const T& value)
{
	updateSize(1);
	assert(activSize <= bufferSize);
	//T * res = new (buffer+size-1) T(value);
	T * res = buffer + activSize - 1;
	*res = value;
	return *res;
}

/*******************  FUNCTION  *********************/
/**
 * Resize the array. It may induce reallocation of the internal buffer
 * if it is too small.
**/
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
		
		buffer = (T*)MALT_REALLOC(buffer,bufferSize * sizeof(T));
	}
}

/*******************  FUNCTION  *********************/
/**
 * Same as setSize but by providing size variation as parameter.
**/
template <class T>
void Array<T>::updateSize(ssize_t delta)
{
	this->setSize(this->activSize+delta);
}

/*******************  FUNCTION  *********************/
/**
 * Remove the last element. It will not reduce the memory usage of the
 * internal buffer.
**/
template <class T>
void Array<T>::pop(void)
{
	assert(activSize > 0);
	this->updateSize(-1);
}

/*******************  FUNCTION  *********************/
/**
 * Change the content of the current array by copying the content from then
 * given one.
 * @param orig Original array to copy.
**/
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
/**
 * Manage conversion to JSON/LUA through htopml::convertToJson functions.
**/
template <class T>
void convertToJson(htopml::JsonState& json, const Array< T >& value)
{
	json.printArray(value.buffer,value.activSize);
}

/*******************  FUNCTION  *********************/
/**
 * Manage affectation through equal operator.
**/
template <class T>
Array<T> & Array<T>::operator=(const Array<T>& orig)
{
	this->set(orig);
	return *this;
}

}

#endif //MALT_ARRAY_H
