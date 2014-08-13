Portability
===========


Code structure
--------------

Functions which might be impacted by portability issue are mostly stored into lib/portability subdirectory.
This directory provide a wrapper header for each class of portable groups :

- Compiler.hpp
- OS.hpp
- Mutex.hpp
- Spinlock.hpp
- ProcMapReader.hpp

Each of them are generic and point to specialized implementation :

- Compiler : CompilerGNU
- OS : OSUnix
- Mutex : MutexPthread, LockDummy
- Spinlock : SpinlockPthread, LockDummy

Selection
---------

Implementation are selected throught variables exported by cmake into src/lib/config.h :

- MATT_PORTABILITY_SPINLOCK_*
- MATT_PORTABILITY_MUTEX_*
- MATT_PORTABILITY_OS_*
- MATT_PORTABILITY_COMPILER_*

