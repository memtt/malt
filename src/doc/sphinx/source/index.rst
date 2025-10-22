.. MALT documentation master file, created by
   sphinx-quickstart on Thu Oct  2 15:33:24 2025.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

MALT documentation
==================

What is it
----------

MALT is a memory tool to find where you allocate your memory. It also provides you some statistics
about memory usage and help to find memory leaks.

It is done to be used on laguages : **C**, **C++**, **Fortran**, **Rust**, **Python** *(expérimental)*.

It comes with a web-based graphical interface to navigate the profile :

.. image:: _static/images/malt-source-annot.png

First time here?
----------------

We have a few places for you to get started:

.. descriptions here are active

:doc:`/start/dependencies`
  Start by installing the **required** or **optional** dependencies.

:doc:`/start/installation`
  Follow the **installation procedure**.

:doc:`/start/basic-usage`
  Make your first steps by **using malt**.

:doc:`/start/metrics`
  The metrics exported by **malt**.

:doc:`/start/infos`
  General informations about **malt**.

Advanced topics
---------------

:doc:`/advanced/mpi`
  Using MALT for **MPI** applications.

:doc:`/advanced/custom-allocs`
  Using MALT with **custom memory allocators**.

:doc:`/advanced/options`
  Using MALT **options**.

:doc:`/advanced/packaging`
  Building **packaging** for common distributions.

.. the global tocs

.. toctree::
   :maxdepth: 1
   :hidden:
   :caption: Getting started

   /start/dependencies.rst
   /start/installation.rst
   /start/basic-usage.rst
   /start/metrics.rst
   /start/infos.rst

.. toctree::
   :maxdepth: 1
   :hidden:
   :caption: Advanced topics

   /advanced/mpi.rst
   /advanced/custom-allocs.rst
   /advanced/options.rst
   /advanced/packaging.rst
