/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_TREE_STACK_TRACER_ENTRY_HPP
#define MATT_TREE_STACK_TRACER_ENTRY_HPP

/********************  HEADERS  *********************/
#include <json/JsonState.h>
#include <common/Debug.hpp>

/*******************  NAMESPACE  ********************/
namespace MATT
{

/********************  STRUCT  **********************/
template <class T>
class TreeStackTracerEntry
{
	public:
		TreeStackTracerEntry(const TreeStackTracerEntry<T> * parent = NULL,void * codeAddr = NULL);
		template <class U> friend bool operator < (const TreeStackTracerEntry<U> & v1,const TreeStackTracerEntry<U> & v2);
		template <class U> friend bool operator ==(const TreeStackTracerEntry<U> & v1,const TreeStackTracerEntry<U> & v2);
		template <class U> friend void convertToJson(htopml::JsonState & json, const TreeStackTracerEntry<U> & value);
		T & getData(void);
		void * getCodeAddr(void) const;
		TreeStackTracerEntry<T> * getParent(void);
	public:
		const TreeStackTracerEntry<T> * parent;
		void * codeAddr;
		T * value;
};

/*******************  FUNCTION  *********************/
template <class T>
TreeStackTracerEntry< T >* TreeStackTracerEntry<T>::getParent(void)
{
	return parent;
}

/*******************  FUNCTION  *********************/
template <class T>
void* TreeStackTracerEntry<T>::getCodeAddr(void) const
{
	return codeAddr;
}

/*******************  FUNCTION  *********************/
template <class T>
T& TreeStackTracerEntry<T>::getData(void)
{
	//setup types
	MATT_ASSERT(this != NULL);
	
	//allocate if need
	if (value == NULL)
		value = new T;

	return *value;
}

/*******************  FUNCTION  *********************/
template <class T>
TreeStackTracerEntry<T>::TreeStackTracerEntry(const MATT::TreeStackTracerEntry<T>* parent, void* codeAddr)
{
	this->parent = parent;
	this->codeAddr = codeAddr;
	this->value = NULL;
}

/*******************  FUNCTION  *********************/
template <class T>
bool operator<(const TreeStackTracerEntry<T>& v1, const TreeStackTracerEntry<T>& v2)
{
	if (v1.parent == v2.parent)
		return v1.codeAddr < v2.codeAddr;
	else
		return v1.parent < v2.parent;
}

/*******************  FUNCTION  *********************/
template <class T>
bool operator==(const TreeStackTracerEntry<T>& v1, const TreeStackTracerEntry<T>& v2)
{
	return (v1.parent == v2.parent) && (v1.codeAddr && v2.codeAddr);
}

/*******************  FUNCTION  *********************/
template <class U>
void convertToJson(htopml::JsonState& json, const TreeStackTracerEntry< U >& value)
{
	json.openStruct();
	const TreeStackTracerEntry<U> * cur = &value;
	json.openFieldArray("stack");
	while (cur != NULL)
	{
		if (cur->codeAddr != NULL)
			json.printValue(cur->codeAddr);
		cur = cur->parent;
	}
	json.closeFieldArray("stack");
	json.printField("infos",*value.value);
	json.closeStruct();
}

}

#endif //MATT_TREE_STACK_TRACER_ENTRY_HPP
