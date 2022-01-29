# htpasswd

CLI tool for managing HTTP Basic Authentication password file.

Basically, this is a Node.js implementation of Apache's [`htpasswd`](https://httpd.apache.org/docs/2.4/programs/htpasswd.html) utility.

[![build](https://github.com/gevorg/htpasswd/workflows/build/badge.svg)](https://github.com/gevorg/htpasswd/actions/workflows/build.yml)

## Installation

Via git (or downloaded tarball):

```bash
$ git clone git://github.com/gevorg/htpasswd.git
```
Via [npm](http://npmjs.org/):

```bash
$ npm install -g htpasswd
```
## Usage

```bash
$ htpasswd [-cimBpsDv] [ -C cost ] passwordfile username
$ htpasswd -b[cmBpsDv] [ -C cost ] passwordfile username password

$ htpasswd -n[imBps] [ -C cost ] username
$ htpasswd -nb[mBps] [ -C cost ] username password
```

## Options

 - `-b` - Use the password from the command line rather than prompting for it. This option should be used with extreme care, since the password is clearly visible on the command line. For script use see the -i option.
 - `-i` - Read the password from stdin without verification (for script usage).
 - `-c` - Create a new file.
 - `-n` - Don't update file; display results on stdout.
 - `-m` - Use MD5 encryption for passwords. This is the default.
 - `-B` - Use bcrypt encryption for passwords. This is currently considered to be very secure.
 - `-C` - This flag is only allowed in combination with -B (bcrypt encryption). It sets the computing time used for the bcrypt algorithm (higher is more secure but slower, default: 5, valid: 4 to 31).
 - `-d` - Use crypt() encryption for passwords. This algorithm limits the password length to 8 characters. This algorithm is insecure by today's standards.
 - `-s` - Use SHA encryption for passwords. This algorithm is insecure by today's standards.
 - `-p` - Do not encrypt the password (plaintext).
 - `-D` - Delete the specified user.
 - `-v` - Verify password. Verify that the given password matches the password of the user stored in the specified htpasswd file.

## Running tests

It uses [mocha](https://mochajs.org/), so just run following command in package directory:

```bash
$ npm test
```

## License

The MIT License (MIT)