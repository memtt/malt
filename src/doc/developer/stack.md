Stack representation
====================


Gloals
------

The internal stack representation is defined by Stack class from stack/ subdirectory. This class
use an internal representation compatible with glibc backtrace() function. The stack
if stored an array of function pointers (void \*). The stack storage order is defined by parameters and
can be ASC or DESC.

The stack object is abstract and is specialized with two subversion which mostly provide implementation
of the loading methods :

 - BacktraceStack : Implementation of the backtrace method. This class only provide implementation of loading
  method through glibc backtrace() function. It use ASC ordering so the most recent frame in use, last element is the
  root of the callstack.
 - EnterExitStack : Similar to backtrace but for use with instrumented mode where MALT is notified on function
   enter/exit. It uses a reverse storage order and provide filling through enter() and exit() methods.

Memory
------

The main objective of the Stack class is to manage the memory buffer to store the stack and automatically grow it
when needed. The memory segment is increased exponentially until a certain size then linearly it is never
shrinked until full deletion.

Hash & ordering operator
------------------------

Stacks provide a basic hashing method to be quickly stored with that key into hash maps. It also implement
an ordering operator help such support. Stack compares is done in size then on contents so it can 
be used to build efficient AVL trees in addition to the hash and it will use full compare only on
a limited number of elements.
