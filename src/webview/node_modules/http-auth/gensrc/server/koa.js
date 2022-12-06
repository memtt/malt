"use strict";

// Export middleware.

Object.defineProperty(exports, "__esModule", {
    value: true
});
exports.middleware = middleware;
function middleware(auth) {
    // Middleware for koa.
    var koa = function koa(req, res, next) {
        auth.check(req, res, function (req, res, err) {
            if (err) {
                throw err;
            } else {
                next();
            }
        });
    };

    // Return middleware.
    return regeneratorRuntime.mark(function _callee(next) {
        return regeneratorRuntime.wrap(function _callee$(_context) {
            while (1) {
                switch (_context.prev = _context.next) {
                    case 0:
                        _context.next = 2;
                        return koa.bind(null, this.req, this.res);

                    case 2:
                        _context.next = 4;
                        return next;

                    case 4:
                    case "end":
                        return _context.stop();
                }
            }
        }, _callee, this);
    });
}