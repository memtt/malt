"use strict";

// HTTPS module.

var _https = require('https');

var _https2 = _interopRequireDefault(_https);

var _base = require('../auth/base');

var _base2 = _interopRequireDefault(_base);

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

// Backup old server creation.
var oldCreateServer = _https2.default.createServer;

// Mutate server.


// Base module.
_https2.default.createServer = function () {
    var _arguments = arguments;

    var server = undefined;

    if (arguments[0] instanceof _base2.default) {
        (function () {
            var auth = _arguments[0];

            if (_arguments[2]) {
                (function () {
                    var listener = _arguments[2];
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

                    // HTTPS options and listener.
                    server = oldCreateServer.apply(_https2.default, [_arguments[1], newListener]);
                })();
            } else {
                // Only HTTPS options.
                server = oldCreateServer.apply(_https2.default, [_arguments[1]]);
                server.on('request', function (req, res) {
                    auth.check(req, res);
                });
            }
        })();
    } else {
        server = oldCreateServer.apply(_https2.default, arguments);
    }

    // Return server.
    return server;
};