.. _packaging:

Packaging for distributions
===========================

MALT provide the necessary script to generate the packages for common distributions.
You will find here the procedure to follow to build them.

Archive from the git sources
----------------------------

Before following the rest of this document you need either to:

- Download an **official release** tarball.
- **Generate** the release tarball.

To generate from the **git sources** :

.. code-block:: shell

    ./dev/dev.py archive --commit v1.5.0 --version 1.5.0 --no-hash

Gentoo
------

On Gentoo you can directly use the Overlay provided on the web : https://github.com/memtt/gentoo-memtt-overlay

The usage procedure is given here :

.. code-block:: shell

    sudo eselect repository add memtt git https://github.com/memtt/gentoo-memtt-overlay.git
    sudo eselect repository enable memtt
    sudo emerge -a malt

RPM : Fedora and others
-----------------------

If you want to build your package for Fedora or related RPM based distribution,
you can follow the given commands.

.. code-block:: shell

    # Extract the packging dir if you are not in git sources
    tar --strip-components 1 -xvf malt-1.5.0.tar.bz2 malt-1.5.0/packaging/fedora

    # build the podman image
    podman build -t malt/fedora-rpmbuild -f packaging/fedora/Dockerfile

    # Call the script inside docker
    podman run --rm -ti -v .:/sources:rw malt/fedora-rpmbuild /sources/packaging/fedora/build.sh 1.5.0

APT : Debian and others
-----------------------

If you want to build your package for Debian, Ubuntu or related APT based distributions,
you can follow the given commands.

.. code-block:: shell

    # Extract the packging dir if you are not in git sources
    tar --strip-components 1 -xvf malt-1.5.0.tar.bz2 malt-1.5.0/packaging/debian

    # build the podman image
    podman build -t malt/debian-debbuild -f packaging/debian/Dockerfile

    # Call the script inside docker
    podman run --rm -ti -v .:/sources:rw malt/debian-debbuild /sources/packaging/debian/build.sh 1.5.0
