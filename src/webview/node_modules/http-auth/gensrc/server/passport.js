"use strict";

// Imports.

Object.defineProperty(exports, "__esModule", {
    value: true
});
exports.strategy = strategy;

var _passport = require('passport');

var _passport2 = _interopRequireDefault(_passport);

var _util = require('util');

var _util2 = _interopRequireDefault(_util);

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

// Define strategy.
var HttpStrategy = function HttpStrategy(auth) {
    this.name = 'http';
    this.authentication = auth;

    _passport2.default.Strategy.call(this);
};

// Inherit basic strategy.
_util2.default.inherits(HttpStrategy, _passport2.default.Strategy);

// Define auth method.
HttpStrategy.prototype.authenticate = function (req) {
    var self = this;

    // Is auth.
    this.authentication.isAuthenticated(req, function (result) {
        if (result instanceof Error) {
            self.error(result);
        } else if (!result.pass) {
            var header = self.authentication.generateHeader(result);
            self.fail(header);
        } else {
            self.success(result.user);
        }
    });
};

// Export.
function strategy(auth) {
    return new HttpStrategy(auth);
}