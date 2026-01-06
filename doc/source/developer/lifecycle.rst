MALT project lifecycle
======================

In order to manage the **lifecycle** of **MALT** there is a specific set of scripts
provided by the `./dev` directory to help the developement, mostly the release time.

Dependences
-----------

In order to use the main entry point you need to first create a venv

.. code:: shell

	# crate a venv the first time
	python3 -m venv .venv

	# load it
	source .venv/bin/activate

	# to use the dev scripts
	pip install -r dev/requirements.txt

	# if you want to be able to build the doc
	pip install -r doc/requirements.txt

Main entry point
----------------

Then you can use the main entry point `./dev/dev.py` :

.. code:: text

	usage: dev [-h] {portability,port,archive,ar,coverage,cov,headers,head,version,ver} ...

	Globale source manager of the MALT repo.

	options:
	  -h, --help            show this help message and exit

	command:
	  {portability,port,archive,ar,coverage,cov,headers,head,version,ver}
	                        Command to apply
	    portability (port)  Run the MALT build & tests in varios Podman images.
	    archive (ar)        Generate delivery archive.
	    coverage (cov)      Generate coverage in current build directory.
	    headers (head)      UPdate file headers.
	    version (ver)       Update version in sources

Portabilty validation
---------------------

MALT comes with a script to test build & unit tests on a set of distributions with
more of less depdencies available. This is usefull before making a release to see
the problems rising on the various distributions.

The script is based on **Podman** to run each distro in a **container**. In order
to be faster it caches the packages and use **ccache** to not loose time when cycling
many time in the script.

All the cached data about this script is stored into the `./podman-cache` directory.

The script validates on :

.. table:: Table of metrics
   :widths: auto

   ===================   ================================================
      Distribution                        Versions
   ===================   ================================================
   Ubuntu                22.04, 24.04, 25.04, 25.10
   Debian                11, 12, 13, sid
   Fedora                40, 41
   Archlinux             latest
   Gentoo                latest stable
   ===================   ================================================

To run it you can use the standard **pytest** options.

.. code:: shell

	# run everything
	./dev/dev.py port

	# run only ubuntu
	./dev/dev.py port -- -k ubuntu

	# run only ubuntu in verbose mode
	./dev/dev.py port -v -- -k ubuntu

	# run only ubuntu 24.04
	./dev/dev.py port -v -- -k 24.04

	# stop on first error
	./dev/dev.py port -v -- -k 24.04 -x

	# stop on first error and start from last failed
	./dev/dev.py port -v -- -k 24.04 -x --lf

Jumping in a podman image
-------------------------

If you want to manually debug in one of the images, you can :

.. code:: shell

	# launching podman on the given image
	podman run --rm -v .:/mnt/sources -ti malt/ubuntu-basic:22.04 /bin/bash

	# inside podman :
	mkdir ~/build
	cd ~/build
	/mnt/sources/configure --enable-debug --enable-tests
	make
	make test

Update file headers
-------------------

The file headers contains the last modification date of the file and the authors
extracted from the git repository.

You can re-generate them by calling :

.. code:: shell

	./dev/dev.py headers --git-all .

When making the commit, **PLEASE** use a name containg `[headers]` so it is ignored
by the script latter.

.. code:: shell

	git commit -a -m "[headers] Update headers"

Updating the version number
---------------------------

When increasiong the version number you can use the script `./dev/dev.py version` :

.. code:: text

	usage: dev version [-h] [--beta] [--dev] version

	positional arguments:
	version     Set the version number

	options:
	  -h, --help  show this help message and exit
	  --beta      Mark the version as beta
	  --dev       Mark the version as dev branch

Making the release archives
---------------------------

In order to generate the release archives you can use directly (be sure to have commit
all your changes, it will use git to generate the archive).

.. code:: shell

	./dev/dev.py ar

By default it generate a version with the hash. If the **tag** of the version if on
**current commit**, then it generate the official final archive.
