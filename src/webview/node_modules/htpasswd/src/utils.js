"use strict";

// Importing crypto module.
const crypto = require("crypto");

// Importing apache-md5 module.
const md5 = require("apache-md5");

// Importing apache-crypt module.
const crypt = require("apache-crypt");

// Bcrypt.
const bcrypt = require("bcryptjs");

// Export object.
const utils = {};

// Generates sha1 hash of password.
utils.sha1 = password => {
  let hash = crypto.createHash("sha1");
  hash.update(password);

  return hash.digest("base64");
};

// Verifies if password is correct.
utils.verify = (hash, password) => {
  if (hash.substr(0, 5) === "{SHA}") {
    hash = hash.substr(5);
    return utils.sha1(password) === hash;
  } else if (hash.substr(0, 6) === "$apr1$" || hash.substr(0, 3) === "$1$") {
    return md5(password, hash) === hash;
  } else if (hash.substr(0, 4) === "$2y$" || hash.substr(0, 4) === "$2a$") {
    return bcrypt.compareSync(password, hash);
  } else {
    return hash === password || crypt(password, hash) === hash;
  }
};

// Encodes password hash for output.
utils.encode = program => {
  if (!program.delete) {
    // Get username and password.
    let password = program.args[program.args.length - 1];

    // Encode.
    if (!program.plaintext) {
      if (program.crypt) {
        password = crypt(password);
      } else if (program.sha) {
        password = "{SHA}" + utils.sha1(password);
      } else if (program.bcrypt) {
        let cost = program.cost ? program.cost : 5;
        let salt = bcrypt.genSaltSync(cost);

        password = bcrypt.hashSync(password, salt);
      } else {
        password = md5(password);
      }
    }

    // Return it.
    return password;
  }
};

// Export.
module.exports = utils;
