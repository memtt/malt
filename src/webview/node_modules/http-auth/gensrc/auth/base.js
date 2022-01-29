"use strict";

// File system module.

Object.defineProperty(exports, "__esModule", {
    value: true
});

var _createClass = function () { function defineProperties(target, props) { for (var i = 0; i < props.length; i++) { var descriptor = props[i]; descriptor.enumerable = descriptor.enumerable || false; descriptor.configurable = true; if ("value" in descriptor) descriptor.writable = true; Object.defineProperty(target, descriptor.key, descriptor); } } return function (Constructor, protoProps, staticProps) { if (protoProps) defineProperties(Constructor.prototype, protoProps); if (staticProps) defineProperties(Constructor, staticProps); return Constructor; }; }();

var _fs = require('fs');

var _fs2 = _interopRequireDefault(_fs);

var _events = require('events');

var _events2 = _interopRequireDefault(_events);

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

function _classCallCheck(instance, Constructor) { if (!(instance instanceof Constructor)) { throw new TypeError("Cannot call a class as a function"); } }

function _possibleConstructorReturn(self, call) { if (!self) { throw new ReferenceError("this hasn't been initialised - super() hasn't been called"); } return call && (typeof call === "object" || typeof call === "function") ? call : self; }

function _inherits(subClass, superClass) { if (typeof superClass !== "function" && superClass !== null) { throw new TypeError("Super expression must either be null or a function, not " + typeof superClass); } subClass.prototype = Object.create(superClass && superClass.prototype, { constructor: { value: subClass, enumerable: false, writable: true, configurable: true } }); if (superClass) Object.setPrototypeOf ? Object.setPrototypeOf(subClass, superClass) : subClass.__proto__ = superClass; }

// Event module.


// Base authentication.
var Base = function (_events$EventEmitter) {
    _inherits(Base, _events$EventEmitter);

    // Constructor.
    function Base(options, checker) {
        _classCallCheck(this, Base);

        var _this = _possibleConstructorReturn(this, (Base.__proto__ || Object.getPrototypeOf(Base)).call(this));

        if (!options.msg401) {
            options.msg401 = "401 Unauthorized";
        }

        if (!options.msg407) {
            options.msg407 = "407 Proxy authentication required";
        }

        if (!options.contentType) {
            options.contentType = "text/plain";
        }

        if (!options.realm) {
            options.realm = "users";
        }

        // Assign values.
        _this.options = options;
        _this.checker = checker;

        // Loading users from file, if file is set.
        _this.options.users = [];

        if (!checker && options.file) {
            _this.loadUsers();
        }
        return _this;
    }

    // Processing user line.


    _createClass(Base, [{
        key: 'processLine',
        value: function processLine(userLine) {
            throw new Error('Not defined!');
        }

        // Parse auth header.

    }, {
        key: 'parseAuthorization',
        value: function parseAuthorization(header) {
            throw new Error('Not defined!');
        }

        // Find user.

    }, {
        key: 'findUser',
        value: function findUser(req, clientOptions, callback) {
            throw new Error('Not defined!');
        }

        // Generates header.

    }, {
        key: 'generateHeader',
        value: function generateHeader(result) {
            throw new Error('Not defined!');
        }

        // Ask for authentication.

    }, {
        key: 'ask',
        value: function ask(res, result) {
            var header = this.generateHeader(result);
            res.setHeader("Content-Type", this.options.contentType);

            if (this.proxy) {
                res.setHeader("Proxy-Authenticate", header);
                res.writeHead(407);
                res.end(this.options.msg407);
            } else {
                res.setHeader("WWW-Authenticate", header);
                res.writeHead(401);
                res.end(this.options.msg401);
            }
        }

        // Checking if user is authenticated.

    }, {
        key: 'check',
        value: function check(req, res, callback) {
            var self = this;
            this.isAuthenticated(req, function (result) {
                if (result instanceof Error) {
                    self.emit('error', result, req);

                    if (callback) {
                        callback.apply(self, [req, res, result]);
                    }
                } else if (!result.pass) {
                    self.emit('fail', result, req);
                    self.ask(res, result);
                } else {
                    self.emit('success', result, req);

                    if (!this.options.skipUser) {
                        req.user = result.user;
                    }

                    if (callback) {
                        callback.apply(self, [req, res]);
                    }
                }
            });
        }

        // Is authenticated method.

    }, {
        key: 'isAuthenticated',
        value: function isAuthenticated(req, callback) {
            var self = this;
            var header = undefined;
            if (this.proxy) {
                header = req.headers["proxy-authorization"];
            } else {
                header = req.headers["authorization"];
            }

            // Searching for user.
            var searching = false;

            // If header is sent.
            if (header) {
                var clientOptions = this.parseAuthorization(header);
                if (clientOptions) {
                    searching = true;
                    this.findUser(req, clientOptions, function (result) {
                        callback.apply(self, [result]);
                    });
                }
            }

            // Only if not searching call callback.
            if (!searching) {
                callback.apply(this, [{}]);
            }
        }

        // Loading files with user details.

    }, {
        key: 'loadUsers',
        value: function loadUsers() {
            var _this2 = this;

            var users = _fs2.default.readFileSync(this.options.file, 'UTF-8').replace(/\r\n/g, "\n").split("\n");

            // Process all users.
            users.forEach(function (u) {
                if (u && !u.match(/^\s*#.*/)) {
                    _this2.processLine(u);
                }
            });
        }
    }]);

    return Base;
}(_events2.default.EventEmitter);

// Export base.


exports.default = Base;