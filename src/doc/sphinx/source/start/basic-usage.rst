Basic usage
===========

Now you have installed **MALT** you can proceed by using it.

Examples directory
------------------

In order to start playing with **MALT** you can find some examples into
the directory `/PREFIX/share/malt/examples`.

Profiling a C application
-------------------------

If you have a C / C++ / Fortran / Rust application, first compile it by using
the `-g` debug option to be able to find the source lines from the binaries.

.. code-block:: shell

    # Compile with debug option -g
    gcc -g -O3 -o basic-example ./main.c

The simply run within **malt** :

.. code-block:: shell

    # Run by wrapping your command with 'malt'
    malt ./basic-example

It will produce a file named `malt-basic-example-PID.json` to be analised offline.

Browsing the profile
--------------------

You can browser into the profile by using the command `malt-webview`.
This command starts a lightweight server locally
so you can connect on it with your web browser onto http://localhost:8080 by default.

Note that it will ask you for a token which is displayed into the terminarl when
the mini-server is ready to render the pages.

.. code-block:: shell

    # run the mini server
    malt-webview ./malt-basic-example-5656.json
    # open your browser on http://localhost:8080.

Fast profiling
--------------

If you want to make a fast profiling without extracting the call stacks (which adds
a large overhead by the tool), you can disable the stacks by using :

.. code-block:: shell

    # Run fast without analysing the stacks
    malt -o stacks:mode=none ./basic-example

It will produce the profile with all the counters except the call tree and source
annotations. By the way it has in this mode a very low overhead to get a first view
on the dynamic of the application.

Profiling python only
---------------------

Since release **1.4.0** malt supports profiling Python scripts. To proceed, simply use
the `malt-python` command instead of `malt`.

.. code-block:: shell

    # profiling python only
    malt-python ./my-script.py

    # read the profile with the webview
    malt-webview ./malt-python-my-script-5656.json

As for the C case, it will produce a file `malt-python-my-script-PID.json` which you open
via the web interface.

Under python the overhead is very hight, if you want to get a first look
on the global metrics, first make a run with the **no-stack** profile.

.. code-block:: shell

    # profiling python only
    malt-python --profile python-no-stack ./my-script.py

Profiling python and C
----------------------

This kind of analysis increase the overhead so it is not enabled by default on
the `malt-python` command. In order to proceed, use the `--profile` option :

.. code-block:: shell

    # see both domains splitted
    malt-python --profile python-domains

    # see both domains merged as a single call tree
    malt-python --profile python-mix
