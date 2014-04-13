/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_STACK_TREE_SUB_SET_HPP
#define MATT_STACK_TREE_SUB_SET_HPP

/********************  HEADERS  *********************/
#include <set>
#include <json/JsonState.h>

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  CLASS  **********************/
template <class T>
class StackTreeSubSetNode
{
	public:
		StackTreeSubSetNode(StackTreeSubSetNode<T> * parent,void * codeAddr);
		template <class U> friend bool operator<(const StackTreeSubSetNode<U> & a,const StackTreeSubSetNode<U> & b);
		template <class U> friend bool operator==(const StackTreeSubSetNode<U> & a,const StackTreeSubSetNode<U> & b);
		template <class U> friend void convertToJson(htopml::JsonState & json, const StackTreeSubSetNode<U> & value);
	public:
		std::set<StackTreeSubSetNode<T> > childs;
		StackTreeSubSetNode<T> * parent;
		void * codeAddr;
		T * value;
};

/*******************  FUNCTION  *********************/
template <class T>
StackTreeSubSetNode<T>::StackTreeSubSetNode(MATT::StackTreeSubSetNode<T>* parent, void* codeAddr)
{
	this->parent = parent;
	this->codeAddr = codeAddr;
	this->value = NULL;
}

/*******************  FUNCTION  *********************/
template <class T>
bool operator<(const StackTreeSubSetNode<T>& v1, const StackTreeSubSetNode<T>& v2)
{
	return v1.codeAddr < v2.codeAddr;
}

/*******************  FUNCTION  *********************/
template <class T>
bool operator==(const StackTreeSubSetNode<T>& v1, const StackTreeSubSetNode<T>& v2)
{
	return (v1.codeAddr && v2.codeAddr);
}

/*******************  FUNCTION  *********************/
template <class U>
void convertToJson(htopml::JsonState& json, const StackTreeSubSetNode< U >& value)
{
	json.openStruct();
	const StackTreeSubSetNode<U> * cur = &value;
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

#endif //MATT_STACK_TREE_SUB_SET_HPP
