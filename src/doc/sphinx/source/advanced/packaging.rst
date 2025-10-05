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

```sh
./dev/dev.py archive --commit v1.4.0 --version 1.4.0 --no-hash
```

Gentoo
------

On Gentoo you can directly use the Overlay provided on the web : https://github.com/memtt/gentoo-memtt-overlay

The usage procedure is also given :ref:`here <gentoo_overlay>`.

RPM : Fedora and others
-----------------------

If you want to build your package for Fedora or related RPM based distribution,
you can follow the given commands.

```sh
# Extract the packging dir if you are not in git sources
tar --strip-components 1 -xvf malt-1.4.0.tar.bz2 malt-1.4.0/packaging/fedora

# build the podman image
podman build -t malt/fedora-rpmbuild -f packaging/fedora/Dockerfile

# Call the script inside docker
podman run --rm -ti -v .:/sources:rw malt/fedora-rpmbuild /sources/packaging/fedora/build.sh 1.4.0
```
