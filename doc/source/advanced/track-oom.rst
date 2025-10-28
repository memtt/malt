Track out of memory
===================

When speaking about memory in HPC, one of the main issue which can arise is
being killed on the computation node due to beeing **out of memory**.

MALT cannot be triggered at the exect moment of the **out of memory** being
triggered. This, because at that moment the application (and the instrumentation)
tool cannot do anything more than being killed.

But MALT can trigger a dump of the profile a little bit before by playing with
some software threasholds so you can in post-mortem analyse the profile as usual.

Available options
-----------------

You can play with the options from the **dump** group of options.

.. table:: Table of metrics
   :widths: auto

   =======================   ================================================
      Option                     Short description
   =======================   ================================================
   `on-signal`               Dump on signal. Can be comma separated list from SIGINT, SIGUSR1,
   `after-seconds`           Dump after X seconds (limited to only one time)
   `on-sys-full-at`          Dump when system memory become full at x%, xG, xM, xK, x  (empty to disable).
   `on-app-using-rss`        Dump when RSS of the app reach the given limit in %, G, M, K (empty to disable).
   `on-app-using-virt`       Dump when Virtual Memory of the app reach limit in %, G, M, K (empty to disable).
   `on-app-using-req`        Dump when Requested Memory of the app reach limit in %, G, M, K (empty to disable).
   `on-thread-stack-using`   Dump when one stack reach limit in %, G, M, K (empty to disable).
   `on-alloc-count`          Dump when number of allocations reach limit in G, M, K (empty to disable).
   `watch-dog`               Run an active thread spying continuouly the memory of the app, not only sometimes.
   =======================   ================================================

recommended approch
-------------------

One recommended way if to play with `dump:on-sys-full-at` and `dump:watch-dog`.
The first option permits de define a threashold when to dump base on the system
memory. The good point is that is also work in a multi-process environnement.

As the system memory is looking at the **physical memory** (**rss**) consumption of the app
if requires to be dynamically tracked via a **wathdog** thread as the peak can be reached
between two calls of malloc which can let MALT missing it before being killed.

For the option `dump:on-sys-full-at`, select a value below `100%` as there can be
side effects before reaching the max (swap....). Also MALT will use a bit a memory
to dump the profile so you need some margins. By experience, **80%** looks a good value.

.. code-block:: shell

    malt -o dump:watch-dog=true -o dump:on-sys-full-at=80% ./my_oom_program

Look in the profile
-------------------

When looking in the profile, you can get the memory used a peak time with the metric
**Global Peak Memory**.