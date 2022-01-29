"use strict";

// Base class.

Object.defineProperty(exports, "__esModule", {
    value: true
});

var _createClass = function () { function defineProperties(target, props) { for (var i = 0; i < props.length; i++) { var descriptor = props[i]; descriptor.enumerable = descriptor.enumerable || false; descriptor.configurable = true; if ("value" in descriptor) descriptor.writable = true; Object.defineProperty(target, descriptor.key, descriptor); } } return function (Constructor, protoProps, staticProps) { if (protoProps) defineProperties(Constructor.prototype, protoProps); if (staticProps) defineProperties(Constructor, staticProps); return Constructor; }; }();

exports.default = function (options, checker) {
    return new Digest(options, checker);
};

var _base = require('./base');

var _base2 = _interopRequireDefault(_base);

var _utils = require('./utils');

var utils = _interopRequireWildcard(_utils);

var _nodeUuid = require('node-uuid');

var _nodeUuid2 = _interopRequireDefault(_nodeUuid);

function _interopRequireWildcard(obj) { if (obj && obj.__esModule) { return obj; } else { var newObj = {}; if (obj != null) { for (var key in obj) { if (Object.prototype.hasOwnProperty.call(obj, key)) newObj[key] = obj[key]; } } newObj.default = obj; return newObj; } }

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

function _classCallCheck(instance, Constructor) { if (!(instance instanceof Constructor)) { throw new TypeError("Cannot call a class as a function"); } }

function _possibleConstructorReturn(self, call) { if (!self) { throw new ReferenceError("this hasn't been initialised - super() hasn't been called"); } return call && (typeof call === "object" || typeof call === "function") ? call : self; }

function _inherits(subClass, superClass) { if (typeof superClass !== "function" && superClass !== null) { throw new TypeError("Super expression must either be null or a function, not " + typeof superClass); } subClass.prototype = Object.create(superClass && superClass.prototype, { constructor: { value: subClass, enumerable: false, writable: true, configurable: true } }); if (superClass) Object.setPrototypeOf ? Object.setPrototypeOf(subClass, superClass) : subClass.__proto__ = superClass; }

// Utility module.


// Unique id.


// Define digest auth.
var Digest = function (_Base) {
    _inherits(Digest, _Base);

    // Constructor.
    function Digest(options, checker) {
        _classCallCheck(this, Digest);

        // Array of random strings sent to clients.
        var _this = _possibleConstructorReturn(this, (Digest.__proto__ || Object.getPrototypeOf(Digest)).call(this, options, checker));

        _this.nonces = [];

        // Algorithm of encryption, could be MD5 or MD5-sess, default is MD5.
        if ('MD5-sess' !== options.algorithm) {
            _this.options.algorithm = "MD5";
        }

        // Quality of protection is by default auth.
        if (options.qop === 'none') {
            _this.options.qop = '';
        } else {
            _this.options.qop = 'auth';
        }
        return _this;
    }

    // Process user line.


    _createClass(Digest, [{
        key: 'processLine',
        value: function processLine(line) {
            var tokens = line.split(":");

            // We need only users for given realm.
            if (this.options.realm === tokens[1]) {
                this.options.users.push({ username: tokens[0], hash: tokens[2] });
            }
        }

        // Parse authorization heder.

    }, {
        key: 'parseAuthorization',
        value: function parseAuthorization(header) {
            var opts = {};
            var parts = header.split(' ');
            var params = parts.slice(1).join(' ');

            // Split the parameters by comma.
            var tokens = params.split(/,(?=(?:[^"]|"[^"]*")*$)/);
            if (parts[0].substr(0, 6) === "Digest") {
                // Parse parameters.
                var i = 0;
                var len = tokens.length;

                while (i < len) {
                    // Strip quotes and whitespace.
                    var param = /(\w+)=["]?([^"]*)["]?$/.exec(tokens[i]);
                    if (param) {
                        opts[param[1]] = param[2];
                    }

                    ++i;
                }
            }

            // Return options.
            return opts;
        }

        // Validating hash.

    }, {
        key: 'validate',
        value: function validate(ha2, co, hash) {
            var ha1 = hash;

            // Algorithm.
            if (co.algorithm === 'MD5-sess') {
                ha1 = utils.md5(ha1 + ':' + co.nonce + ':' + co.cnonce);
            }

            var response = undefined;

            // Quality of protection.
            if (co.qop) {
                response = utils.md5(ha1 + ':' + co.nonce + ':' + co.nc + ':' + co.cnonce + ':' + co.qop + ':' + ha2);
            } else {
                response = utils.md5(ha1 + ':' + co.nonce + ':' + ha2);
            }

            // If calculated response is equal to client's response.
            return response === co.response;
        }

        // Searching for user.

    }, {
        key: 'findUser',
        value: function findUser(req, co, callback) {
            var _this2 = this;

            var self = this;

            if (this.validateNonce(co.nonce, co.qop, co.nc)) {
                (function () {
                    var ha2 = utils.md5(req.method + ':' + co.uri);
                    if (_this2.checker) {
                        // Custom authentication.
                        _this2.checker.apply(_this2, [co.username, function (hash) {
                            var params = undefined;

                            if (hash instanceof Error) {
                                params = [hash];
                            } else {
                                params = [{ user: co.username, pass: !!self.validate(ha2, co, hash) }];
                            }

                            // Call callback.
                            callback.apply(this, params);
                        }, req]);
                    } else {
                        var pass = false;

                        // File based, loop users to find the matching one.
                        _this2.options.users.forEach(function (user) {
                            if (user.username === co.username && _this2.validate(ha2, co, user.hash)) {
                                pass = true;
                            }
                        });

                        callback.apply(_this2, [{ user: co.username, pass: pass }]);
                    }
                })();
            } else {
                callback.apply(this, [{ stale: true }]);
            }
        }

        // Remove nonces.

    }, {
        key: 'removeNonces',
        value: function removeNonces(noncesToRemove) {
            var _this3 = this;

            noncesToRemove.forEach(function (nonce) {
                var index = _this3.nonces.indexOf(nonce);
                if (index != -1) {
                    _this3.nonces.splice(index, 1);
                }
            });
        }

        // Validate nonce.

    }, {
        key: 'validateNonce',
        value: function validateNonce(nonce, qop, nc) {
            var found = false;

            // Current time.
            var now = Date.now();

            // Nonces for removal.
            var noncesToRemove = [];

            // Searching for not expired ones.
            this.nonces.forEach(function (serverNonce) {
                if (serverNonce[1] + 3600000 > now) {
                    if (serverNonce[0] === nonce) {
                        if (qop) {
                            if (nc > serverNonce[2]) {
                                found = true;
                                ++serverNonce[2];
                            }
                        } else {
                            found = true;
                        }
                    }
                } else {
                    noncesToRemove.push(serverNonce);
                }
            });

            // Remove expired nonces.
            this.removeNonces(noncesToRemove);

            return found;
        }

        // Generates and returns new random nonce.

    }, {
        key: 'askNonce',
        value: function askNonce() {
            var nonce = utils.md5(_nodeUuid2.default.v4());
            this.nonces.push([nonce, Date.now(), 0]);

            return nonce;
        }

        // Generates request header.

    }, {
        key: 'generateHeader',
        value: function generateHeader(result) {
            var nonce = this.askNonce();
            var stale = result.stale ? true : false;

            // Returning it.
            return 'Digest realm="' + this.options.realm + '", qop="' + this.options.qop + '", nonce="' + nonce + '", algorithm="' + this.options.algorithm + '", stale="' + stale + '"';
        }
    }]);

    return Digest;
}(_base2.default);

// Export digest auth.