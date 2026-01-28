Static report
=============

In some circumstances, you might want to dump a **static** version of the **webview**
so you can use it anywhere without the server. You can also **archive** it or **embed**
the static report in some **other tools**.

In that case you can use the `malt-webview` command like this :

.. code:: shell

	# generate all static available pages in the report
	malt-webview --static ./report  ./malt-my_progr-01256.json

	# generate only the static summary page.
	malt-webview --static-summary ./report ./malt-my_progr-01256.json

Unsupported pages
-----------------

As they require the server of need to handle a subsequent part of the json which can be too big for the browser,
the **static** version of the report contains all the pages except :

 - The **source browser** and **annotated sources**.
 - The **call tree** page.
 - The **stack memory usage** page.

They are replaced by a page with the **instructions** on how to get MALT and use the profile to look at them.

Embed the profile
-----------------

The report can also optionnaly embed the profile so it can be reused latter.
You can embed it as compressed or uncompressed.

.. code:: shell

	# generate all static available pages in the report
	malt-webview --static --embed-profile xz ./report  ./malt-my_progr-01256.json

	# generate only the static summary page.
	malt-webview --static-summary --embed-profile xz ./report ./malt-my_progr-01256.json

Possible values for the option `-e/--embed-profile` are :

 - **xz**: to compress the profile with `xz`.
 - **json**: to keep it uncompressed.
 - **none**: to skip.
