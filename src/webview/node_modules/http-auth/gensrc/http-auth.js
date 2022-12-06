"use strict";

// Utils.

var _utils = require('./auth/utils');

var utils = _interopRequireWildcard(_utils);

require('./server/http');

require('./server/https');

function _interopRequireWildcard(obj) { if (obj && obj.__esModule) { return obj; } else { var newObj = {}; if (obj != null) { for (var key in obj) { if (Object.prototype.hasOwnProperty.call(obj, key)) newObj[key] = obj[key]; } } newObj.default = obj; return newObj; } }

// http-proxy integration.


// http integration.
if (utils.isAvailable('http-proxy')) {
    require('./server/proxy');
}

// Exports.


// https integration.
module.exports = {
    // Basic authentication.
    basic: function basic(options, checker) {
        return require('./auth/basic').default(options, checker);
    },

    // Digest authentication.
    digest: function digest(options, checker) {
        return require('./auth/digest').default(options, checker);
    },

    // Connect.
    connect: function connect(auth) {
        return require('./server/connect').middleware(auth);
    },

    // Koa.
    koa: function koa(auth) {
        return require('./server/koa').middleware(auth);
    },

    // Passport.
    passport: function passport(auth) {
        return require('./server/passport').strategy(auth);
    }
};