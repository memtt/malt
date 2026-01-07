Sub-part of a program
=====================

If you run on a really big program doing millions of allocation you might get a big overhead, and maybe
you are just interested in a sub-part of the program. You can do it by including `malt/malt.h` in
your files and use `malt_enable()` an `malt_disable()` to controle MALT on each thread. It is also a nice
way to detect leaks of sub-parts of your code.

.. code:: c

   #include <malt/malt.h>

   int main()
   {
       malt_disable();
       //ignored
       malloc(16);

       malt_enable();
       //tracked
       malloc(16);
   }

You will need to link the `libmalt-controler.so` to get the default fake symbols when not using MALT.
You can also just provide the two empty functions in your own dynamic library (not static).

If you have some allocation not under your control before your first call you can disable MALT by default
on threads using the `filter:enabled` option, then enable it by hand.

If you want to change the initial status (make it disabled at start), you can simply redefine in your program
the function :

.. code:: c

    #include <stdlib.h>
    #include <malt/malt.h>

    //malt will be disabled at start
    int malt_init_status(void)
    {
        return 0;
    }

    int main()
    {
        //perform some costly code

        malt_enable();
        //perform some costly code you want to analyse
        malt_disable();

        //ok
        return EXIT_SUCCESS;
    }
