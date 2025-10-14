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