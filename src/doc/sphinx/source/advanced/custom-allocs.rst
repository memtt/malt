Using custom allocator
======================

In some cases, you application might be using a **custom memory allocator**. Here there
is two cases :

1. The application is replacing the default allocator by overriding the officiel `malloc`, `free`...
In this case, nothing to do as long as the replacement allocator is provided as a **dynamic library**
so **malt** can be **hooked in**.
2. The applicatoin use the **standard allocator** for most of the code but use one or several **custom one**
for **some parts**.

In the second case, you need to perform some dedicated actions.

Standard API with prefix
------------------------

If your memory allocator natively just export the standard memory allocator API with just a **prefix**
in from of all function names of the **API**, then you can simply proceed by :

Lets consider the case where you have the given correspondances :

.. table:: Table of metrics
   :widths: auto

   ===================   ================================================
      Custom API                    Corresponding standard API
   ===================   ================================================
   `je_malloc`           `malloc`
   `je_calloc`           `calloc`
   `je_free`             `free`
   `je_realloc`          `realloc`
   `je_memalign`         `memalign`
   `je_posix_memalign`   `posix_memalign`
   `je_aligned_alloc`    `aligned_alloc`
   ===================   ================================================

.. code-block:: shell

    # wrapping some custom allocator
    malt --wrap-prefix je_

    # wrapping several custom allocators
    malt --wrap-prefix je_ --wrap-prefix mi

**Note** it will work only if the given memory allocators are build as a **shared library** outside
of the application otherwise there is no ways to be **hooked in** from outside by overriding the symbols.

Non standard API
----------------

If for some reason your API does not provide exacly the list of functions listed in previous
function, you need to list the wrappers one by one.

**Note** it still requires that each function follow the same API for the related symbols.

.. code-block:: shell

    # wrapping only some part of the allocator because not all provided
    malt --wrap malloc:je_malloc,free:je_free
