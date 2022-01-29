"use strict";

// Des module.

var _unixCryptTdJs = require("unix-crypt-td-js");

var _unixCryptTdJs2 = _interopRequireDefault(_unixCryptTdJs);

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

// Hash generation string.
var itoa64 = "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

// Salt generation method.
function getSalt() {
    return itoa64[parseInt(Math.random() * 64)] + itoa64[parseInt(Math.random() * 64)];
}

// Exporting old style.
module.exports = function (password, salt) {
    return salt ? (0, _unixCryptTdJs2.default)(password, salt) : (0, _unixCryptTdJs2.default)(password, getSalt());
};