'use strict';
module.exports = function (scope) {
	var rc = require('rc')('npm', {registry: 'https://registry.npmjs.org/'});
	return rc[scope + ':registry'] || rc.registry;
};
