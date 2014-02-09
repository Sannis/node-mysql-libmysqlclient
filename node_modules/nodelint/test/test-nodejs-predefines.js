/*
 * Nodelint tests for Node.js predefines working correctly
 *
 * Released into the Public Domain by tav <tav@espians.com>
 * See the README.md for full credits of the awesome contributors!
 */

/**
 * Module dependencies
 */
var helper = require('./helper');

/**
 * Test for Node.js predefines working correctly
 *
 * @param test
 */
exports.NodePredefines = function (test) {
  helper.testConsoleOutput(
    './nodelint',
    [__dirname + '/fixtures/nodejs-predefines.js'],
    {
      stdout: '0 errors\n',
      stderr: '',
      exitCode: 0
    },
    test
  );
};

