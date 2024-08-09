'use strict';

// core modules
const stream = require('stream');
const util = require('util');

// external modules
const assert = require('assert-plus');
const intoStream = require('into-stream');
const JSONStream = require('JSONStream');
const through2 = require('through2');
const once = require('once').strict;
const JsonStreamStringify = require('json-stream-stringify');

// promisified implementations of callback APIs.
const _parsePromisified = util.promisify(_parse);
const _stringifyPromisified = util.promisify(_stringify);

/**
 * Create a JSON.parse that uses a stream interface. The underlying
 * implementation is handled by JSONStream. This is merely a thin wrapper for
 * convenience that handles the reconstruction/accumulation of each
 * individually parsed field.
 *
 * The advantage of this approach is that by also using a streams interface,
 * any JSON parsing or stringification of large objects won't block the CPU.
 * @public
 * @return {Stream}
 */
function createParseStream() {
    // when the parse stream gets chunks of data, it is an object with key/val
    // fields. accumulate the parsed fields.
    let accumulator = null;
    const parseStream = JSONStream.parse('$*');
    const wrapperStream = through2.obj(
        function write(chunk, enc, cb) {
            // try to be clever (oh noes). assume we parse objects by default.
            // if the stream starts and it looks like an array, set the
            // starting value of the accumulator to an array. we opt into the
            // array, with default accumulator as an object. this introduces
            // less risk with this feature for any unexpected circumstances
            // (hopefully).
            if (accumulator === null) {
                const chunkStr = chunk.toString(enc).trim();
                // if the trimmed chunk is an empty string, delay initialization
                // of the accumulator till we get something meaningful
                if (chunkStr !== '') {
                    if (chunkStr.charAt(0) === '[') {
                        accumulator = [];
                    } else {
                        accumulator = {};
                    }
                }
            }
            parseStream.write(chunk);
            return cb();
        },
        function flush(cb) {
            parseStream.on('end', function() {
                return cb(null, accumulator);
            });
            parseStream.end();
        }
    );

    parseStream.on('data', function(chunk) {
        // this syntax should work when accumulator is object or array
        accumulator[chunk.key] = chunk.value;
    });

    // make sure error is forwarded on to wrapper stream.
    parseStream.on('error', function(err) {
        wrapperStream.emit('error', err);
    });

    return wrapperStream;
}

/**
 * create a JSON.stringify readable stream.
 * @public
 * @param {Object} opts an options object
 * @param {Object} opts.body the JS object to JSON.stringify
 * @function createStringifyStream
 * @return {Stream}
 */
function createStringifyStream(opts) {
    assert.object(opts, 'opts');
    assert.ok(
        Array.isArray(opts.body) || typeof opts.body === 'object',
        'opts.body must be an array or object'
    );

    return new JsonStreamStringify(opts.body, null, null, false);
}

/**
 * stream based JSON.parse. async function signature to abstract over streams.
 * @public
 * @param {Object} opts options to pass to parse stream
 * @param {String|Buffer} opts.body string or buffer to parse
 * @param {Function} callback a callback function
 * @return {Object|Array} the parsed JSON
 */
function _parse(opts, callback) {
    assert.object(opts, 'opts');
    assert.ok(
        typeof opts.body === 'string' || Buffer.isBuffer(opts.body),
        'opts.body'
    );
    assert.func(callback, 'callback');

    const sourceStream = intoStream(opts.body);
    const parseStream = createParseStream();
    const cb = once(callback);

    parseStream.on('data', function(data) {
        return cb(null, data);
    });

    parseStream.on('error', function(err) {
        return cb(err);
    });

    sourceStream.pipe(parseStream);
}

/**
 * stream based JSON.parse. async function signature to abstract over streams.
 * variadic arguments to support both promise and callback based usage.
 * @public
 * @function parse
 * @param {Object} opts options to pass to parse stream
 * @param {String} opts.body string to parse
 * @param {Function} [callback] a callback function. if empty, returns a
 * promise.
 * @return {Object|Array} the parsed JSON
 */
function parse(opts, callback) {
    // if more than one argument was passed, assume it's a callback based usage.
    if (arguments.length > 1) {
        return _parse(opts, callback);
    }

    // otherwise, caller expects a promise.
    return _parsePromisified(opts);
}

/**
 * stream based JSON.stringify. async function signature to abstract over
 * streams.
 * @private
 * @param {Object} opts options to pass to stringify stream
 * @param {Function} callback a callback function
 * @return {Object} the parsed JSON object
 */
function _stringify(opts, callback) {
    assert.object(opts, 'opts');
    assert.func(callback, 'callback');

    let stringified = '';
    const stringifyStream = createStringifyStream(opts);
    const passthroughStream = new stream.PassThrough();
    const cb = once(callback);

    // setup the passthrough stream as a sink
    passthroughStream.on('data', function(chunk) {
        stringified += chunk;
    });

    passthroughStream.on('end', function() {
        return cb(null, stringified);
    });

    // don't know what errors stringify stream may emit, but pass them back
    // up.
    stringifyStream.on('error', function(err) {
        return cb(err);
    });

    stringifyStream.pipe(passthroughStream);
}

/**
 * stream based JSON.stringify. async function signature to abstract over
 * streams. variadic arguments to support both promise and callback based usage.
 * @public
 * @function stringify
 * @param {Object} opts options to pass to stringify stream
 * @param {Function} [callback] a callback function. if empty, returns a
 * promise.
 * @return {Object} the parsed JSON object
 */
function stringify(opts, callback) {
    // if more than one argument was passed, assume it's a callback based usage.
    if (arguments.length > 1) {
        return _stringify(opts, callback);
    }

    // otherwise, caller expects a promise.
    return _stringifyPromisified(opts);
}

module.exports = {
    createParseStream,
    createStringifyStream,
    parse,
    stringify
};
