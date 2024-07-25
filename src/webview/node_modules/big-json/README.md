# big-json

[![NPM Version](https://img.shields.io/npm/v/big-json.svg)](https://npmjs.org/package/big-json)
[![Build Status](https://travis-ci.org/DonutEspresso/big-json.svg?branch=master)](https://travis-ci.org/DonutEspresso/big-json)
[![Coverage Status](https://coveralls.io/repos/github/DonutEspresso/big-json/badge.svg?branch=master)](https://coveralls.io/github/DonutEspresso/big-json?branch=master)
[![Dependency Status](https://david-dm.org/DonutEspresso/big-json.svg)](https://david-dm.org/DonutEspresso/big-json)
[![devDependency Status](https://david-dm.org/DonutEspresso/big-json/dev-status.svg)](https://david-dm.org/DonutEspresso/big-json#info=devDependencies)

> A stream based implementation of JSON.parse and JSON.stringify for big POJOs

There exist many stream based implementations of JSON parsing or stringifying
for large data sets. These implementations typical target time series data, new
line delimited data or other array-like data, e.g., logging records or other
continuous flowing data.

This module hopes to fill a gap in the ecosystem: parsing large JSON objects
that are just _really_ big objects. With large in-memory objects, it is
possible to run up against the V8 string length limitation, which is currently
(as of 9/2017) limited to 512MB. Thus, if your large object has enough keys
or values, it is possible to exceed the string length limit when calling
[JSON.stringify](https://github.com/nodejs/node/issues/10738).

Similarly, when retrieving stored JSON from disk or over the network, if the
JSON stringified representation of the object exceeds the string length limit,
the process will throw when attempting to convert the Buffer into a string.

The only way to work with such large objects is to use a streaming
implementation of both `JSON.parse` and `JSON.stringify`. This module does just
that by normalizing the APIs for different modules that have previously
published, combining both parse and stringify functions into a single module.
These underlying modules are subject to change at anytime.

The major caveat is that the reconstructed POJO must be able to fit in memory.
If the reconstructed POJO cannot be stored in memory, then it may be time to
reconsider the way these large objects are being transported and processed.

This module currently uses
[JSONStream](https://github.com/dominictarr/JSONStream) for parsing, and
[json-stream-stringify](https://github.com/Faleij/json-stream-stringify) for
stringification.

## Getting Started

Install the module with: `npm install big-json`

## Usage

To parse a big JSON coming from an external source:

```js
const fs = require('fs');
const path = require('path');
const json = require('big-json');

const readStream = fs.createReadStream('big.json');
const parseStream = json.createParseStream();

parseStream.on('data', function(pojo) {
    // => receive reconstructed POJO
});

readStream.pipe(parseStream);
```

To stringify JSON:
```js
const json = require('big-json');

const stringifyStream = json.createStringifyStream({
    body: BIG_POJO
});

stringifyStream.on('data', function(strChunk) {
    // => BIG_POJO will be sent out in JSON chunks as the object is traversed
});
```


## API

### createParseStream()
Parses an incoming stream and accumulates it into a POJO. Supports both objects
and arrays as root objects for stream data.

__Returns__: {Stream} a JSON.parse stream

### createStringifyStream(opts)

* `opts` {Object} an options object
* `opts.body` {Object | Array} an object or array to JSON.stringify

__Returns__: {Stream} a JSON.stringify stream

### parse(opts, [callback])
An async JSON.parse using the same underlying stream implementation. If a
callback is not passed, a promise is returned.

* `opts` {Object} an options object
* `opts.body` {String | Buffer} the string or buffer to be parsed
* `callback` {Function} a callback object

__Returns__: {Object | Array} the parsed JSON

### stringify(opts, [callback])
An async JSON.stringify using the same underlying stream implementation. If a
callback is not passed, a promise is returned.

* `opts` {Object} an options object
* `opts.body` {Object} the object to be stringified
* `callback` {Function} a callback object

__Returns__: {Object} the stringified object

## Contributing

Ensure that all linting and codestyle tasks are passing. Add unit tests for any
new or changed functionality.

To start contributing, install the git prepush hooks:

```sh
make githooks
```

Before committing, lint and test your code using the included Makefile:
```sh
make prepush
```

## License

Copyright (c) 2019 Alex Liu

Licensed under the MIT license.
