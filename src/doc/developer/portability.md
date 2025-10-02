Portability
===========


Code structure
--------------

Functions which might be impacted by portability issues are mostly stored into lib/portability subdirectory.
This directory provides a wrapper header for each class of portable groups :

- Compiler.hpp
- OS.hpp
- Mutex.hpp
- Spinlock.hpp
- ProcMapReader.hpp

Each of them is generic and points to specialized implementation :

- Compiler : CompilerGNU
- OS : OSUnix
- Mutex : MutexPthread, LockDummy
- Spinlock : SpinlockPthread, LockDummy

Selection
---------

Implementation are selected through variables exported by cmake into src/lib/config.h :

- MALT\_PORTABILITY\_SPINLOCK\_\* : Provide spinlock wrapper, by default use pthread implementation but might be other
  for example on windows. Malt also provide a Dummy verison.
- MALT\_PORTABILITY\_MUTEX\_\* : Provide spinlock wrapper, by default use pthread implementation but might be other
  for example on windows. Malt also provides a Dummy version.
- MALT\_PORTABILITY\_OS\_\* : Provide some OS specific routines. Currently only implement Unix version.
- MALT\_PORTABILITY\_COMPILER\_\* : Provide some compiler specific routines. Currently only provide GCC/ICC for C++
  demangling.
