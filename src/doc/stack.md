Stack representation
====================


Gloals
------

The internal stack representation is defined by Stack class from stack/ subdirectory. This class
use an internal representation compatible with glibc backtrace() function. The stack
if stored a an array of function pointers (void \*). The stack storage order is defined by parameter and
can be ASC or DESC.

The stack object is abstract and is specialized with two subversion which mostly provide implementation
of the loading methods :

 - BacktraceStack : Implementation for the backtrace method. This class only provide implementation of loading
  method through glibc backtrace() function. It use ASC ordering so the most recent frame in use, last element is the
  root of the callstack.
 - EnterExitStack : Similar to backtrace but for use with instrumented mode where MATT is notified on function
   enter/exit. It use a reverse storage order and provide filling through enter() and exit() methods.

Memory
------

The main objectiv of the Stack class is to manage the memoy buffer to store the stack and automatically grow it
when needed. The memory segment is increased exponentially unitil a certain size then linearly it is never
shrinked until full deletion.

Hash & ordering operator
------------------------

Stacks provide a basic hashing mehod to be quicly stored with that key into hash maps. It also implement
an ordering operator the help such support. Stack compare is done on size then on contents so it can 
be used to build efficient AVL trees in addition to the hash and it will use full compare only on
limited number of elements.

Generic entries
---------------

MATT use an abstract tree representation that can contain any types of data as long as we defined a related
descriptor at init time. First we need to allocate a tree implementation, there is currently three solutions :

	#include <core/options.hpp>
	#include <stacks/StackTreeMap.hpp>
	#include <stacks/RLockFreeTree.hpp>
	#include <allocators/NoFreeAllocator.hpp>

	Options & options = getOptions();
	void * ptr;
	switch(options.getStackMode())
	{
		case MATT_STACK_MAP_BACKTRACE:
			ptr = MATT_NO_FREE_MALLOC(sizeof(StackTreeMap));
			this->stackTree = new (ptr) StackTreeMap();
			break;
		case MATT_STACK_TREE_ENTER_EXIT:
			ptr = MATT_NO_FREE_MALLOC(sizeof(RLockFreeTree));
			this->stackTree = new (ptr) RLockFreeTree();
			break;
		case MATT_STACK_MAP_ENTER_EXIT:
			ptr = MATT_NO_FREE_MALLOC(sizeof(StackTreeMap));
			this->stackTree = new (ptr) StackTreeMap(false);
			break;
		default:
			MATT_FATAL_ARG("Invalid stck tree mode from options : %1 ! ").arg(options.getStackMode()).end();
	}

Now you can setup the descriptors by providing a type and an entry name :
	
	#include <stacks/StackTree.hpp>
	
	//setup tree
	int id = this->stackTree->addDescriptor<CallCounter>("alloc");
	assumeArg(id == MATT_ANA_ID_ALLOC,"Do not get valid ID while registering descriptors : %1 != %2").arg(id).arg(MATT_ANA_ID_ALLOC).end();

