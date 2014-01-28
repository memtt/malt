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

/********************  STRUCT  **********************/
class TreeStackTracerEntry
{
	public:
		TreeStackTracerEntry(TreeStackTracerEntry * parent = NULL,void * codeAddr = NULL);
		friend bool operator < (const TreeStackTracerEntry & v1,const TreeStackTracerEntry & v2);
		friend bool operator ==(const TreeStackTracerEntry & v1,const TreeStackTracerEntry & v2);
	private:
		TreeStackTracerEntry * parent;
		void * codeAddr;
};

/*******************  FUNCTION  *********************/
inline TreeStackTracerEntry::TreeStackTracerEntry(TreeStackTracerEntry* parent, void* codeAddr)
{
	this->parent = parent;
	this->codeAddr = codeAddr;
}

/*******************  FUNCTION  *********************/
inline bool TreeStackTracerEntry::operator<(const TreeStackTracerEntry& v1, const TreeStackTracerEntry& v2)
{
	if (v1.parent == v2.parent)
		return v1.codeAddr < v2.codeAddr;
	else
		return v1.parent < v2.parent;
}

/*******************  FUNCTION  *********************/
inline bool TreeStackTracerEntry::operator==(const TreeStackTracerEntry& v1, const TreeStackTracerEntry& v2)
{
	return (v1.parent == v2.parent) && (v1.codeAddr && v2.codeAddr);
}

#endif //MATT_TREE_STACK_TRACER_ENTRY_HPP
