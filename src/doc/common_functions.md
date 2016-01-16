Common functions of library
===========================

Messages
--------

To print error messages with objects formatting you can use the FormattedMessage class. It is used
as bases class for Debug messages defined into common/Debug.h. Debug messages can be quickly displayed
with :

 MALT_FATAL("Describe error");

If you need arguments :

 MALT_FATAL_ARG("Describe error with arg %1 and %2").arg(x).arg(y).end()

Please do not forget the call to end() method to print the message and apply exit method (abort or raise exception...)

Message levels are :

- Debug
- Fatal
- Error
- Warning
- Message
- Info

Assert / assume
---------------

As assert() do malloc() on failure, we recommend using the MALT\_ASSERT() macro to avoid entering into
an infinite loop into MALT internal methods called inside malloc/calloc...

	#include <common/Debug.h>
	
	MALT_ASSERT(check == true);

Assertions are disabled for production but MALT provides a second macro (assume and assumeArg) which
stay active but need an error message.

	#include <common/Debug.h>
	
	assumeArg(ok == true,"Failed to get OK, get %1").arg(ok).end();

When checking commands from glibc, you can easily use the error message from errno with :

	#include <common/Debug.h>
	
	FILE * fp = ....
	assumeArg("Failed to open file %1 : %2").arg(file).argStrErrno().end();

Locking
-------

At low level MALT provide two wrapper classes for locks : Mutex and Spinlock which can be used as :

	#include <portability/Mutex.hpp>
	
	Mutex mutex;
	
	mutex.lock();
	//do stuff
	mutex.unlock();

Same for spinlocks :

	#include <portability/Spinlock.hpp>
	
	Spinlock spinlock;
	
	spinlock.lock();
	//do stuff
	spinlock.unlock();

You can use RAII model through the TakeLock class (included by Mutex and Spinlock) to you
haven't to deal manually with lock/unlock. It declares code sections protected by lock
and automatically unlock when exiting the section by using destructor properties.
This scheme permit to support return/exception inside critical sections.

	#include <portability/Mutex.hpp>
	//also work transparently with Spinclock
	
	
	Mutex mutex;
	MALT_CRITICAL(mutex)
	//you stuff
	
	//error can be handled with return inside critical section
	//it will automatically unlock before exiting function
	if (error)
	return;
	MALT_END_CRITICAL

You can also use the optional variant which use or not use the lock depending on a boolean
parameter :

	#include <portability/Spinlock.hpp>
	
	Spinlock lock;
	MALT_OPTIONAL_CRITICAL(lock,isThreadSafe)
	//do your stuff here
	MALT_END_CRITICAL

Internal memory allocations
---------------------------

MALT track memory allocation so we prefer to delegate internal allocation to a separate allocator not to change the
memory layout of the profiled application. This allocator is implemented into SimpleAllocator.[ch]pp and can be used
as :

	#include <common/SimpleAllocator.hpp>
	
	void * ptr = MALT_MALLOC(16);
	MALT_REALLOC(ptr,32);
	MALT_FREE(ptr);

You can wrap this allocator into STL object with :

	#include <common/STLInternalAllocator.hpp>
	
	std::list< int , STLInternalAllocator<int> > myList;

Code timings
------------

The common function provides some macro to quickly time some internal functions of MALT. Timings can be used by wrapping
your code with :

	#include <common/CodeTimings.hpp>
	
	//will automatically print the results at exit
	//Timing only take place if MALT_ENABLE_CODE_TIMING is defined
	//It's enabled by cmake with -DENABLE_CODE_TIMING=yes
	CODE_TIMING("name", you_function());

Caution, there are some bugs due to at_exit() dead lock when instrumenting some programs so it’s a feature which
is mostly buggy with the LD_PRELOAD approach. But its most for developer usage so...

