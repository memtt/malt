Managing the webview code
=========================

The source directory src/webview contain all the sources used by the web GUI and
the webserver hosted by NodeJS. This directory fetch the dependencies with NPM.

Dependencies
------------

The dependencies are defined in the `package.js` file in this directory. You can fetch 
them (already done in the repository) for update with :

```shell
	npm install
```

Static (client-side) dependencies are managed by Bower tool also binded into npm package js
to be automatically called.

Build the compressed files
--------------------------

If you want to embedded the MALT GUI in another server you might want to build
a smaller version of the code. This can be done thanks to the Grunt script also
provided in that directory.

First install the dev dependencies (might already be done if you previously execute npm install) :

```shell
	npm install --dev
```

Then run the grunt script :

```shell
	grunt
```

It will fill the `dist` directory with the packed script file and css file.