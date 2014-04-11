/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_TREE_STACK_TRACER_ENTRY_V3_HPP
#define MATT_TREE_STACK_TRACER_ENTRY_V3_HPP

/********************  HEADERS  *********************/
#include <set>
#include <json/JsonState.h>

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  CLASS  **********************/
template <class T>
class TreeStackTracerEntryV3
{
	public:
		TreeStackTracerEntryV3(TreeStackTracerEntryV3<T> * parent,void * codeAddr);
		template <class U> friend bool operator<(const TreeStackTracerEntryV3<U> & a,const TreeStackTracerEntryV3<U> & b);
		template <class U> friend bool operator==(const TreeStackTracerEntryV3<U> & a,const TreeStackTracerEntryV3<U> & b);
		template <class U> friend void convertToJson(htopml::JsonState & json, const TreeStackTracerEntryV3<U> & value);
	public:
		std::set<TreeStackTracerEntryV3<T> > childs;
		TreeStackTracerEntryV3<T> * parent;
		void * codeAddr;
		T * value;
};

/*******************  FUNCTION  *********************/
template <class T>
TreeStackTracerEntryV3<T>::TreeStackTracerEntryV3(MATT::TreeStackTracerEntryV3<T>* parent, void* codeAddr)
{
	this->parent = parent;
	this->codeAddr = codeAddr;
	this->value = NULL;
}

/*******************  FUNCTION  *********************/
template <class T>
bool operator<(const TreeStackTracerEntryV3<T>& v1, const TreeStackTracerEntryV3<T>& v2)
{
	return v1.codeAddr < v2.codeAddr;
}

/*******************  FUNCTION  *********************/
template <class T>
bool operator==(const TreeStackTracerEntryV3<T>& v1, const TreeStackTracerEntryV3<T>& v2)
{
	return (v1.codeAddr && v2.codeAddr);
}

/*******************  FUNCTION  *********************/
template <class U>
void convertToJson(htopml::JsonState& json, const TreeStackTracerEntryV3< U >& value)
{
	json.openStruct();
	const TreeStackTracerEntryV3<U> * cur = &value;
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

#endif //MATT_TREE_STACK_TRACER_ENTRY_V3_HPP
