"use strict";

// Proxy module.

var _httpProxy = require('http-proxy');

var _httpProxy2 = _interopRequireDefault(_httpProxy);

var _base = require('../auth/base');

var _base2 = _interopRequireDefault(_base);

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

// Backup old server creation.
var oldCreateServer = _httpProxy2.default.createServer;

// New create server.


// Base module.
var newCreateServer = function newCreateServer(auth, options) {
    // Mutated mode.
    if (auth instanceof _base2.default) {
        // Set proxy flag.
        auth.proxy = true;
    } else {
        // Set correct options.
        options = auth;

        // Clear authentication value.
        auth = null;
    }

    // Default listener plus authentication check.
    var server = oldCreateServer.apply(_httpProxy2.default, [options]);

    // Authentication provided.
    if (auth) {
        (function () {
            // Override proxyRequest.
            var oldProxyRequest = server.web;
            server.web = function (req, res) {
                // Fetch external arguments.
                var externalArguments = arguments;

                // Check for authentication.
                auth.check(req, res, function (req, res, err) {
                    if (err) {
                        console.error(err);
                        res.statusCode = 400;
                        res.end(err.message);
                    } else {
                        oldProxyRequest.apply(server, externalArguments);
                    }
                });
            };
        })();
    }

    // Return server.
    return server;
};

// Add authentication method.
_httpProxy2.default.createServer = _httpProxy2.default.createProxyServer = _httpProxy2.default.createProxy = newCreateServer;