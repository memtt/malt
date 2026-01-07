MPI
===

This section will describe how to perform analysis of an **MPI application**.

Profiling an MPI application
----------------------------

If you want to profile an MPI application **MALT** will dump a profile for **each process** so
each **rank**. But by default the files will be named with the **PID** of the process which is
not what you expect in this particular case.

In order to get the rank, you first need to prepare a wrapper to replace the **PID** by the **rank**.

**Notice** it should be redone avery time you use a different MPI flavor.

.. code-block:: shell

    # using mpicxx
    malt --prep-mpi

    # if you use another mpi C++ compiler
    malt --prep-mpi my_mpi_cxx_compiler

Then when **launching** your **mpi** application :

.. code-block:: shell

    mpirun -np 16 malt --mpi ./my_program

Filter do dump only some ranks
------------------------------

In MPI you might know that some ranks are behaving exactly the same way. In this
case you can dump only a set of them :

.. code-block:: shell

    mpirun -np 16 malt --mpi -o filter:ranks=0,5-8,10 ./my_program

