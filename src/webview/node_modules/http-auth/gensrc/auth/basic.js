"use strict";

// Base class.

Object.defineProperty(exports, "__esModule", {
    value: true
});

var _createClass = function () { function defineProperties(target, props) { for (var i = 0; i < props.length; i++) { var descriptor = props[i]; descriptor.enumerable = descriptor.enumerable || false; descriptor.configurable = true; if ("value" in descriptor) descriptor.writable = true; Object.defineProperty(target, descriptor.key, descriptor); } } return function (Constructor, protoProps, staticProps) { if (protoProps) defineProperties(Constructor.prototype, protoProps); if (staticProps) defineProperties(Constructor, staticProps); return Constructor; }; }();

exports.default = function (options, checker) {
    return new Basic(options, checker);
};

var _base = require('./base');

var _base2 = _interopRequireDefault(_base);

var _utils = require('./utils');

var utils = _interopRequireWildcard(_utils);

var _apacheMd = require('apache-md5');

var _apacheMd2 = _interopRequireDefault(_apacheMd);

var _apacheCrypt = require('apache-crypt');

var _apacheCrypt2 = _interopRequireDefault(_apacheCrypt);

function _interopRequireWildcard(obj) { if (obj && obj.__esModule) { return obj; } else { var newObj = {}; if (obj != null) { for (var key in obj) { if (Object.prototype.hasOwnProperty.call(obj, key)) newObj[key] = obj[key]; } } newObj.default = obj; return newObj; } }

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

function _classCallCheck(instance, Constructor) { if (!(instance instanceof Constructor)) { throw new TypeError("Cannot call a class as a function"); } }

function _possibleConstructorReturn(self, call) { if (!self) { throw new ReferenceError("this hasn't been initialised - super() hasn't been called"); } return call && (typeof call === "object" || typeof call === "function") ? call : self; }

function _inherits(subClass, superClass) { if (typeof superClass !== "function" && superClass !== null) { throw new TypeError("Super expression must either be null or a function, not " + typeof superClass); } subClass.prototype = Object.create(superClass && superClass.prototype, { constructor: { value: subClass, enumerable: false, writable: true, configurable: true } }); if (superClass) Object.setPrototypeOf ? Object.setPrototypeOf(subClass, superClass) : subClass.__proto__ = superClass; }

// Utility module.


// Importing apache-md5 module.


// Importing apache-crypt module.


// Define basic auth.
var Basic = function (_Base) {
    _inherits(Basic, _Base);

    // Constructor.
    function Basic(options, checker) {
        _classCallCheck(this, Basic);

        return _possibleConstructorReturn(this, (Basic.__proto__ || Object.getPrototypeOf(Basic)).call(this, options, checker));
    }

    // Verifies if password is correct.


    _createClass(Basic, [{
        key: 'validate',
        value: function validate(hash, password) {
            if (hash.substr(0, 5) === '{SHA}') {
                hash = hash.substr(5);
                return hash === utils.sha1(password);
            } else if (hash.substr(0, 6) === '$apr1$' || hash.substr(0, 3) === '$1$') {
                return hash === (0, _apacheMd2.default)(password, hash);
            } else {
                return hash === password || hash === (0, _apacheCrypt2.default)(password, hash);
            }
        }

        // Processes line from authentication file.

    }, {
        key: 'processLine',
        value: function processLine(userLine) {
            var lineSplit = userLine.split(":");
            var username = lineSplit.shift();
            var hash = lineSplit.join(":");

            // Push user.
            this.options.users.push({ username: username, hash: hash });
        }

        // Generates request header.

    }, {
        key: 'generateHeader',
        value: function generateHeader() {
            return 'Basic realm="' + this.options.realm + '"';
        }

        // Parsing authorization header.

    }, {
        key: 'parseAuthorization',
        value: function parseAuthorization(header) {
            var tokens = header.split(" ");
            if (tokens[0] === "Basic") {
                return tokens[1];
            }
        }

        // Searching for user.

    }, {
        key: 'findUser',
        value: function findUser(req, hash, callback) {
            var _this2 = this;

            // Decode base64.
            var splitHash = utils.decodeBase64(hash).split(":");
            var username = splitHash.shift();
            var password = splitHash.join(":");

            var self = this;

            if (this.checker) {
                // Custom auth.
                this.checker.apply(this, [username, password, function (result) {
                    var params = undefined;

                    if (result instanceof Error) {
                        params = [result];
                    } else {
                        params = [{ user: username, pass: !!result }];
                    }

                    callback.apply(self, params);
                }, req]);
            } else {
                // File based auth.
                var pass = false;

                // Loop users to find the matching one.
                this.options.users.forEach(function (user) {
                    if (user.username === username && _this2.validate(user.hash, password)) {
                        pass = true;
                    }
                });

                // Call final callback.
                callback.apply(this, [{ user: username, pass: pass }]);
            }
        }
    }]);

    return Basic;
}(_base2.default);

// Export basic auth.