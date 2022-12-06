"use strict";

// Importing crypto module.

Object.defineProperty(exports, "__esModule", {
    value: true
});
exports.md5 = md5;
exports.sha1 = sha1;
exports.base64 = base64;
exports.decodeBase64 = decodeBase64;
exports.isAvailable = isAvailable;

var _crypto = require('crypto');

var _crypto2 = _interopRequireDefault(_crypto);

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

// Generates md5 hash of input.
function md5(input) {
    var hash = _crypto2.default.createHash('MD5');
    hash.update(input);

    return hash.digest('hex');
}

// Generates sha1 hash of input.
function sha1(input) {
    var hash = _crypto2.default.createHash('sha1');
    hash.update(input);

    return hash.digest('base64');
}

// Encode to base64 string.
function base64(input) {
    return new Buffer(input, 'utf8').toString('base64');
}

// Decodes base64 string.
function decodeBase64(input) {
    return new Buffer(input, 'base64').toString('utf8');
}

// Check if module is available.
function isAvailable(path) {
    try {
        return !!require.resolve(path);
    } catch (err) {
        return false;
    }
}