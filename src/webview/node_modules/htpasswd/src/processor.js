"use strict";

// Utils.
const utils = require("./utils");

// FS.
const fs = require("fs");

// Prompt module.
const prompt = require("prompt");

// Export object.
const processor = {};

// Validates input arguments.
processor.validate = program => {
  // Only username is required.
  let expectedArgs = 1;

  if (program.batch) {
    ++expectedArgs;
  }

  if (!program.nofile) {
    ++expectedArgs;
  }

  if (program.cost) {
    program.cost = +program.args[0];
    program.args = program.args.slice(1);
  }

  // Validate argument count.
  return program.args.length === expectedArgs;
};

// Sync file with new data.
processor.syncFile = program => {
  const passwordFile = program.args[0];
  const username = program.args[1];
  const password = program.args[2];

  // Hash encode.
  const hash = utils.encode(program);

  // Collector params.
  let found = false;
  let newLines = [];

  if (!program.create) {
    if (!fs.existsSync(passwordFile)) {
      console.error(
        `Cannot modify file ${passwordFile}; use '-c' to create it.`
      );
      return;
    }

    const lines = fs.readFileSync(passwordFile, "UTF-8").split("\n");

    // Loop lines.
    lines.forEach(line => {
      if (line.indexOf(`${username}:`) === 0) {
        found = true;

        if (program.verify) {
          // For verification we need existing data.
          let hash = line.split(":")[1];
          if (utils.verify(hash, password)) {
            console.log(`Password for user ${username} correct.`);
          } else {
            console.log("Password verification failed.");
          }
        } else if (program.delete) {
          console.log(`Deleting password for user ${username}.`);
        } else {
          // Updating password.
          newLines.push(`${username}:${hash}`);
          console.log(`Updating password for user ${username}.`);
        }
      } else if (line) {
        newLines.push(line);
      }
    });
  }

  if (!program.verify) {
    if (!found) {
      if (program.delete) {
        console.error(`User ${username} not found.`);
      } else {
        newLines.push(`${username}:${hash}`);
        console.log(`Adding password for user ${username}.`);
      }
    }

    // Write data.
    fs.writeFileSync(passwordFile, newLines.join("\n") + "\n", "UTF-8");
  }
};

// Finalizes processing by printing output or changing password file.
processor.finalize = program => {
  if (program.nofile) {
    const username = program.args[0];
    const hash = utils.encode(program);

    // Print to stdout.
    console.log(`${username}:${hash}`);
  } else {
    try {
      processor.syncFile(program);
    } catch (err) {
      console.error(err.message);
    }
  }
};

// Read password from stdin.
function readPasswordStdIn(program) {
  let password = "";

  // Reading pass.
  process.stdin.on("data", function(chunk) {
    password += chunk;
  });

  // Finished reading.
  process.stdin.on("end", function() {
    program.args.push(password);
    processor.finalize(program);
  });
}

// Read password.
function readPassword(program) {
  // Prepare prompt.
  prompt.message = "";
  prompt.delimiter = "";

  const passportOption = [
    { name: "password", description: "New password:", hidden: true }
  ];
  const rePassportOption = [
    { name: "rePassword", description: "Re-type new password:", hidden: true }
  ];

  // Try to read password.
  prompt.get(passportOption, function(err, result) {
    if (!err) {
      const password = result.password;
      setTimeout(function() {
        prompt.get(rePassportOption, function(err, result) {
          if (!err && password == result.rePassword) {
            program.args.push(password);
            processor.finalize(program);
          } else {
            console.error("\nPassword verification error.");
          }
        });
      }, 50);
    } else {
      console.error("\nPassword verification error.");
    }
  });
}

// Processing command.
processor.exec = program => {
  if (processor.validate(program)) {
    if (program.stdin) {
      readPasswordStdIn(program);
    } else if (!program.batch && !program.delete) {
      readPassword(program);
    } else {
      processor.finalize(program);
    }
  } else {
    program.help();
  }
};

// Export processor.
module.exports = processor;
