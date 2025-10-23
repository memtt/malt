Build options
=============

Available options
-----------------

MALT supports several build options to tune MALT at build time. Those options
are listed here :

.. table:: Table of metrics
   :widths: auto

   ===================== ===========  =============================================
           Metric          Default           Short description
   ===================== ===========  =============================================
   ENABLE_TESTS          no           Enable build of internal tests.
   ENABLE_GCC_COVERAGE   no           Enable coverage compile flags to be enabled.
   ENABLE_LINK_OPTIM     yes          Enable link optimisation (LTO & -fvisibility-hidden).
   ENABLE_IPO            yes          Allow usage of IPO specfic optimization when ENABLE_LINK_OPTIM is enabled.
   ENABLE_PYTHON         yes          Enable compiling with the python support.
   ENABLE_JEMALLOC       yes          Enable building internal JeMalloc.
   ENABLE_CACHING        yes          Enable some internal caches.
   ENABLE_PREFER_EMBED   no           Prefer using the embeded dependencies that using system one by default.
   ENABLE_CODE_TIMING    no           Enable some tricks to quickly check timings of hostspots of MALT internal code.
   ENABLE_CODE_LEAK      no           Enable internal analysis of memory leak.
   PORTABILITY_SPINLOCK  PTHREAD      Select spinlock implementation to use.
   PORTABILITY_MUTEX     PTHREAD      Select mutex implementation to use.
   PORTABILITY_OS        UNIX         Select OS implementation to use.
   PORTABILITY_COMPILER  GNU          Select compiler support to use. 
   PORTABILITY_CLOCK     RDTSC        Clock to be used to represent time in MALT.
   PORTABILITY_PYTHON    LAZY         Select the instrumentation mode for python.
   WEBVIEW_SERVER        AUTO         Select the webview server to use.
   ===================== ===========  =============================================

General build options
---------------------

Option ENABLE_TESTS
^^^^^^^^^^^^^^^^^^^

Enable build of internal unit tests.

**Default**: no.

.. code-block:: shell

    cmake .. -DENABLE_TESTS=no
    cmake .. -DENABLE_TESTS=yes
    ../configure --enable-tests
    ../configure --disable-tests

Option ENABLE_GCC_COVERAGE
^^^^^^^^^^^^^^^^^^^^^^^^^^

Enable coverage compile flags to be enabled.

**Default**: no.

.. code-block:: shell

    cmake .. -DENABLE_GCC_COVERAGE=no
    cmake .. -DENABLE_GCC_COVERAGE=yes
    ../configure --enable-gcc-coverage
    ../configure --disable-gcc-coverage

Option ENABLE_PREFER_EMBED
^^^^^^^^^^^^^^^^^^^^^^^^^^

Prefer using the embeded dependencies that using system one by default.

**Default**: no.

.. code-block:: shell

    cmake .. -DENABLE_PREFER_EMBED=no
    cmake .. -DENABLE_PREFER_EMBED=yes
    ../configure --enable-prefer-embed
    ../configure --disable-prefer-embed

Optimisation options
--------------------

Option ENABLE_LINK_OPTIM
^^^^^^^^^^^^^^^^^^^^^^^^

Enable link optimisation (LTO & -fvisibility-hidden).

**Note**: it is not compatible with compilation of unit tests and be disabled
automatically when unit tests are enabled.

**Default**: yes.

.. code-block:: shell

    cmake .. -DENABLE_LINK_OPTIM=no
    cmake .. -DENABLE_LINK_OPTIM=yes
    ../configure --enable-link-optim
    ../configure --disable-link-optim

Option ENABLE_IPO
^^^^^^^^^^^^^^^^^

Allow usage of IPO specfic optimization when ENABLE_LINK_OPTIM is enabled.

**Default**: yes.

.. code-block:: shell

    cmake .. -DENABLE_IPO=no
    cmake .. -DENABLE_IPO=yes
    ../configure --enable-ipo
    ../configure --disable-ipo

Feature options
---------------

Option ENABLE_PYTHON
^^^^^^^^^^^^^^^^^^^^

Enable compiling with the python support. In order to be enabled, it requires
the presence the the **Python C header files**.

**Default**: yes.

.. code-block:: shell

    cmake .. -DENABLE_PYTHON=no
    cmake .. -DENABLE_PYTHON=yes
    ../configure --enable-python
    ../configure --disable-python

Option ENABLE_JEMALLOC
^^^^^^^^^^^^^^^^^^^^^^

Enable building internal JeMalloc.

It permits to boost MALT and reduce its internal
memory overhead by being more efficient than the lightweight fallback internal
memory allocator.

**Default**: yes.

.. code-block:: shell

    cmake .. -DENABLE_JEMALLOC=no
    cmake .. -DENABLE_JEMALLOC=yes
    ../configure --enable-jemalloc
    ../configure --disable-jemalloc

.. 
 ENABLE_CACHING        yes      Enable some internal caches.)
 #Code options
 ENABLE_CODE_TIMING    no       Enable some tricks to quickly check timings of hostspots of MALT internal code.)
 ENABLE_CODE_LEAK      no       Enable internal analysis of memory leak.)
 #select portability mode
 PORTABILITY_SPINLOCK  PTHREAD  Select spinlock implementation to use. Currently support : 'PTHREAD', 'DUMMY'.)
 PORTABILITY_MUTEX     PTHREAD  Select mutex implementation to use. Currently support : 'PTHREAD', 'DUMMY'.)
 PORTABILITY_OS        UNIX     Select OS implementation to use. Currently support : 'UNIX'.)
 PORTABILITY_COMPILER  GNU      Select compiler support to use. Currently support : 'GNU' (also valid intel).)
 PORTABILITY_CLOCK     RDTSC    Clock to be used to represent time in MALT.)
 PORTABILITY_PYTHON    LAZY     Select the instrumentation mode for python : 'LAZY', 'NATIVE'.)
 #select webvew server mode
 WEBVIEW_SERVER        AUTO     Select the webview server to use : AUTO, CPP, NODEJS, RUST, OFF.)
