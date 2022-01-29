"use strict";

// Exporting connect integration.

Object.defineProperty(exports, "__esModule", {
    value: true
});
exports.middleware = middleware;
function middleware(auth) {
    return function (req, res, next) {
        auth.check(req, res, function (req, res, err) {
            if (err) {
                next(err);
            } else {
                next();
            }
        });
    };
}