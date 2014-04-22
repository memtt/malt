/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_STACK_MAP_HPP
#define MATT_STACK_MAP_HPP

/********************  HEADERS  *********************/
//extern
#include <map>
#include <vector>
#include <ostream>
#include <stacks/Stack.hpp>
#include <common/STLInternalAllocator.hpp>
#include <json/JsonState.h>
// #include <json/TypeToJson.h>
//locals

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  CLASS  **********************/
class StackSTLMapAbstract
{
	public:
		typedef std::pair<const Stack *,void*> Node;
		typedef std::vector<Node,STLInternalAllocator<Node> > InternalVector;
		typedef std::map<StackHash,InternalVector,std::less<StackHash>,STLInternalAllocator<std::pair<StackHash,InternalVector> > > InternalMap;
	public:
		StackSTLMapAbstract(void);
		virtual ~StackSTLMapAbstract(void);
		void resolveSymbols(SymbolResolver & symbolResolver);
		void clear();
	protected:
		void * getValue(const Stack & stack,int skipDepth = 0);
		Node getNode(const Stack & stack,int skipDepth = 0);
		Stack * copyStack(const Stack & stack,int skipDepth = 0);
		virtual void * allocateObject(void) const = 0;
		virtual void deleteObject(void * ptr) const = 0;
		virtual void printJsonValue(htopml::JsonState & json,const Stack * stack,void * value) const = 0;
	public:
		friend void convertToJson(htopml::JsonState & json, const StackSTLMapAbstract & value);
	private:
		InternalMap map;
};

/*********************  CLASS  **********************/
template <class T>
class StackSTLMap : public StackSTLMapAbstract
{
	public:
		struct Node
		{
			const Stack * stack;
			T * value;
		};
	public:
		virtual ~StackSTLMap(void);
		T & getValueRef(const Stack & stack,int skipDepth = 0);
		T & operator[](const Stack & stack);
		Node getNode(const Stack & stack,int skipDepth = 0);
	public:
		template <class U> friend void convertToJson(htopml::JsonState & json, const StackSTLMap<U> & value);
	protected:
		virtual void* allocateObject(void ) const;
		virtual void deleteObject(void* ) const;
		virtual void printJsonValue(htopml::JsonState& json, const Stack * stack, void* value) const;
};

}

/********************  HEADERS  *********************/
#include "StackSTLMap_impl.hpp"

#endif //MATT_STACK_MAP_HPP
