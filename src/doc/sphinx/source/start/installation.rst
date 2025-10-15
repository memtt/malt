Installation
============

Compiling
---------

To **compile** MALT, simply use :

If you are using **Gentoo** :

.. code-block:: shell

    mkdir build
    cd build
    ../configure --prefix=$HOME/usr-malt
    make -j8
    make install

Note that the **configure** script is just a **wrapper** with the **configure** classic semantic on top of **cmake**, so
you can also call directly **cmake**:

.. code-block:: shell

    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=$HOME/usr-malt -DCMAKE_BUILD_TYPE=Release
    make -j8
    make install

Note about Intel Compiler
-------------------------

MALT is written in C++ so you might possibly encounterd some issue with you build it with GCC and
profile applications built with Intel Compiler. In most cases it should work out of the box without
any issues.

But, I got once an error report about that. In that case, try to compile MALT also with intel compiler
instead if GCC to match the app :

.. code-block:: shell

    # with the configure script
    ../configure CC=icc CXX=icpc
    make

    # with the cmake script directly
    cmake -DCMAKE_C_COMPILER=icc -DCMAKE_CXX_COMPILER=icpc

Installing via spack
--------------------

If you are using `Spack <https://spack.io/>`_ you can simply use it to build the **dependencies** and **MALT** :

.. code-block:: shell

    ./bin/spack install malt

Installing via Gentoo Overlay
-----------------------------

On Gentoo you can also directly install MALT by using the given **overleay** :

.. code-block:: shell

    sudo eselect repository add memtt git https://github.com/memtt/gentoo-memtt-overlay.git
    sudo eselect repository enable memtt
    sudo emerge -a malt

Building distribution packages
------------------------------

MALT provide the files necessary to produce the **APT** and **RPM** packages, you will
find the procedure to use them in the :ref:`advanced section <packaging>`.