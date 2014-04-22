/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_STACK_MAP_IMPL_HPP
#define MATT_STACK_MAP_IMPL_HPP

/********************  HEADERS  *********************/
//internal
#include "StackSTLMapList.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*******************  FUNCTION  *********************/
template <class T>
void* StackSTLMapList<T>::allocateObject(void ) const
{
	void * ptr = MATT_MALLOC(sizeof(T));
	return new (ptr)T();
}

/*******************  FUNCTION  *********************/
template <class T>
void StackSTLMapList<T>::deleteObject(void* ptr) const
{
	T * obj = (T*)ptr;
	obj->~T();
	MATT_FREE(ptr);
}

/*******************  FUNCTION  *********************/
template <class T>
StackSTLMapList<T>::~StackSTLMapList(void )
{
	this->clear();
}

/*******************  FUNCTION  *********************/
template <class T>
typename StackSTLMapList<T>::Node StackSTLMapList<T>::getNode(const Stack& stack, int skipDepth)
{
	StackSTLMapListAbstract::Node tmp = StackSTLMapListAbstract::getNode(stack,skipDepth);
	typename StackSTLMapList<T>::Node res = {tmp.first,tmp.second};
	return res;
}

/*******************  FUNCTION  *********************/
template <class T>
T& StackSTLMapList<T>::getValueRef(const Stack& stack, int skipDepth)
{
	return *(T*)StackSTLMapListAbstract::getValue(stack,skipDepth);
}

/*******************  FUNCTION  *********************/
template <class T>
T& StackSTLMapList<T>::operator[](const Stack& stack)
{
	return getValueRef(stack);
}

/*******************  FUNCTION  *********************/
template <class U>
void convertToJson(htopml::JsonState& json, const StackSTLMapList< U >& value)
{
	const StackSTLMapListAbstract & ref = value;
	convertToJson(json, ref);
}

/*******************  FUNCTION  *********************/
template <class T>
void StackSTLMapList<T>::printJsonValue(htopml::JsonState& json, const Stack * stack, void* value) const
{
	Node node = {stack,(T*)value};
	json.openStruct();
	json.printField("stack",*stack);
	json.printField("infos",*(T*)value);
	json.closeStruct();
}

}

#endif //MATT_STACK_MAP_IMPL_HPP
