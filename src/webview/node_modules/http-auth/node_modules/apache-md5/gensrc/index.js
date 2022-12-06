"use strict";

// Crypto module import.

var _crypto = require('crypto');

var _crypto2 = _interopRequireDefault(_crypto);

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

// Hash generation string.
var itoa64 = './0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz';

// To 64 bit version.
function to64(index, count) {
    var result = '';

    while (--count >= 0) {
        // Result char count.
        result += itoa64[index & 63]; // Get corresponding char.
        index = index >> 6; // Move to next one.
    }

    return result;
}

// Returns salt.
function getSalt(inputSalt) {
    var salt = '';

    if (inputSalt) {
        // Remove $apr1$ token and extract salt.
        salt = inputSalt.split('$')[2];
    } else {
        while (salt.length < 8) {
            // Random 8 chars.
            var rchIndex = Math.floor(Math.random() * 64);
            salt += itoa64[rchIndex];
        }
    }

    return salt;
}

// Returns password.
function getPassword(final) {
    // Encrypted pass.
    var epass = '';

    epass += to64(final.charCodeAt(0) << 16 | final.charCodeAt(6) << 8 | final.charCodeAt(12), 4);
    epass += to64(final.charCodeAt(1) << 16 | final.charCodeAt(7) << 8 | final.charCodeAt(13), 4);
    epass += to64(final.charCodeAt(2) << 16 | final.charCodeAt(8) << 8 | final.charCodeAt(14), 4);
    epass += to64(final.charCodeAt(3) << 16 | final.charCodeAt(9) << 8 | final.charCodeAt(15), 4);
    epass += to64(final.charCodeAt(4) << 16 | final.charCodeAt(10) << 8 | final.charCodeAt(5), 4);
    epass += to64(final.charCodeAt(11), 2);

    return epass;
}

// Exporting old style.
module.exports = function (password, salt) {
    var magic = '';
    if (salt && salt.split('$')[1] === '1') {
        magic = '$1$';
    } else {
        magic = '$apr1$';
    }

    salt = getSalt(salt);

    var ctx = password + magic + salt;
    var final = _crypto2.default.createHash('md5').update(password + salt + password, 'ascii').digest('binary');

    for (var pl = password.length; pl > 0; pl -= 16) {
        ctx += final.substr(0, pl > 16 ? 16 : pl);
    }

    for (var i = password.length; i; i >>= 1) {
        if (i % 2) {
            ctx += String.fromCharCode(0);
        } else {
            ctx += password.charAt(0);
        }
    }

    final = _crypto2.default.createHash('md5').update(ctx, 'ascii').digest('binary');

    // 1000 loop.
    for (var _i = 0; _i < 1000; ++_i) {
        // Weird stuff.
        var ctxl = '';

        if (_i % 2) {
            ctxl += password;
        } else {
            ctxl += final.substr(0, 16);
        }

        if (_i % 3) {
            ctxl += salt;
        }

        if (_i % 7) {
            ctxl += password;
        }

        if (_i % 2) {
            ctxl += final.substr(0, 16);
        } else {
            ctxl += password;
        }

        // Final assignment after each loop.
        final = _crypto2.default.createHash('md5').update(ctxl, 'ascii').digest('binary');
    }

    return magic + salt + '$' + getPassword(final);
};