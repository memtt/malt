How to debug MALT
=================

In order to **debug** MALT if there is a crash (segfault) you need to use some
special tricks to handle the `LD_PRELOAD` of the library into the program via some
**GDB** commands.

Standard run : interactive
--------------------------

In order to ease you life **malt** already provide an option to make it on demand.

.. code:: shell

	malt --gdb ./my_program

With MPI : auto-backtrace
-------------------------

If if arise by using MPI, you might want the **GDB** : `run`, `backtrace` and `exit`
commands to be done automatically for every process.

This if done via :

.. code:: shell

	mpirun -np 4 malt --mpi --gdb-bt ./my_program

With MPI : Get in terminals
---------------------------

In some case you want to **unmix** the output which, by default are merged in a
**single stream**. For this just use :

.. code:: shell

	mpirun -np 4 xterm -e "malt --mpi --gdb ./my_program"
	# or
	mpirun -np 4 xterm -e "malt --mpi --gdb-bt ./my_program"
