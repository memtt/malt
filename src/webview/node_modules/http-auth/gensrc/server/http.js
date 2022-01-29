"use strict";

// HTTP module.

var _http = require('http');

var _http2 = _interopRequireDefault(_http);

var _base = require('../auth/base');

var _base2 = _interopRequireDefault(_base);

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

// Backup old server creation.
var oldCreateServer = _http2.default.createServer;

// Add authentication method.


// Base module.
_http2.default.createServer = function () {
    var _arguments = arguments;

    var server = undefined;

    // Mutated mode.
    if (arguments[0] instanceof _base2.default) {
        (function () {
            var auth = _arguments[0];

            // With listener.
            if (_arguments[1]) {
                (function () {
                    var listener = _arguments[1];
                    var newListener = function newListener(req, res) {
                        auth.check(req, res, function (req, res, err) {
                            if (err) {
                                console.error(err);
                                res.statusCode = 400;
                                res.end(err.message);
                            } else {
                                listener(req, res);
                            }
                        });
                    };

                    // Mutate server.
                    server = oldCreateServer.apply(_http2.default, [newListener]);
                })();
            } else {
                // Without listener.
                server = oldCreateServer.apply(_http2.default, []);
                server.on('request', function (req, res) {
                    auth.check(req, res);
                });
            }
        })();
    } else {
        server = oldCreateServer.apply(_http2.default, arguments);
    }

    // Return server.
    return server;
};