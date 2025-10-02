Dependencies
============

Here are the dependencies for **building** from source and **running** MALT.

Required dependencies
---------------------

The dependencies of MALT are the following if you use the **official releases** which embed some extra files compared to the **GIT repository**:

`Binutils <https://www.gnu.org/software/binutils/>`_
  - 2.24 - 2.38
  - Required for the **nm** and **addr2line** tools to solve the symbols and get the global variables informations.
`CMake <https://cmake.org/>`
  - 2.8 - 3.28.3
  - Required to **build** MALT from source.

Optional dependencies
---------------------

Those dependencies are not required but will permit to **enable some features** support of MALT.

`OpenSSL <https://www.openssl-library.org/source/index.html>`_
  - 0.128 - 0.183
  - Required if you want to have the graphical interface installed in order to handle the authentication via **libcrypto** to hash the passwords.
`libelf <http://www.mr511.de/software/english.html>`_
  - 0.128 - 0.183
  - To extract **global variables** list from executables and libraries.
`libunwind <http://www.nongnu.org/libunwind/>`_
  - 3.11 - 3.13
  - An alternative implementation of glibc **backtrace** method
`libpython <https://www.python.org/>`_
  - 3.11 - 3.14
  - To activate the support of **Python** language in MALT.
`Graphviz <https://graphviz.org/>`_
  - 2.42.2
  - To activate the **call graph rendering** in the webview via the **dot** command.

Embeded dependencies
--------------------

Some dependencies are **embeded** into the GIT repository to ease installation of MALT because
most of the time not present by default on available systems. **If present** MALT will use the
**system version**.

`httplib <https://raw.githubusercontent.com/yhirose/cpp-httplib>`_
  - 0.23.1
  - To implement the C++ based **webview server** REST API.
`GoogleTest <https://google.github.io/googletest/>`_
  - 1.14.0
  - To build and run the MALT **unit tests**.
`Nlohmann-json <https://github.com/nlohmann/json>`_
  - 3.12.0
  - Use to decode / encoder the **JSON** data in the **webview**.

Dependencies for the GIT repo
-----------------------------

If you want to use the **GIT repository** you might also need some extra
dependences to get some files not embeded in not embeded into the
**GIT repository** but present in the **officiel release archive** :

`NodeJS <https://nodejs.org>`_
  - 0.10.30 - 12.22.9
  - Required if you want to download the **interface dependences** via **npm** which are packed into the officiel release.
`Curl <https://curl.se/download.html>`_
  - 8.5.0
  - Required to **download** the sources of **JeMalloc** memory allocator. Those sources are packed in the the official release.

Dependencies in distributions
-----------------------------

If you want to install the dependencies provided by your system you can easily use the following commands.

Debian / Ubuntu
^^^^^^^^^^^^^^^

If you are using an **APT** based distribution derivated from **Debian** you can use :

.. code-block:: shell

    sudo apt install cmake g++ make libssl-dev libunwind-dev \
                     libelf-dev libunwind-dev nodejs npm \
                     nlohmann-json3-dev graphviz python3-dev curl

Fedora / Rocky
^^^^^^^^^^^^^^

If you are using an **RPM** based distribution derivated from **Fedora** you can use :

.. code-block:: shell

    sudo dnf install -y cmake gcc-c++ make \
                        openssl-devel elfutils-libelf-devel \
                        nodejs npm curl bzip2 xz graphviz python3-devel

Archlinux
^^^^^^^^^

If you are using **Archlinux** :

.. code-block:: shell

    pacman -Sy cmake gcc make openssl \
               libunwind libelf nodejs npm curl \
               nlohmann-json graphviz python3

Gentoo
^^^^^^

If you are using **Gentoo** :

.. code-block:: shell

    emerge cmake gcc make openssl sys-libs/libunwind elfutils \
           nodejs curl dev-cpp/nlohmann_json media-gfx/graphviz
