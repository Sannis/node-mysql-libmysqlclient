/*
 * Nodelint tests for multiple files linting
 *
 * Released into the Public Domain by tav <tav@espians.com>
 * See the README.md for full credits of the awesome contributors!
 */

/**
 * Module dependencies
 */
var helper = require('./helper');

/**
 * Test for multiple files linting
 *
 * @param test
 */
exports.VariousIndents = function (test) {
  helper.testConsoleOutput(
    './nodelint',
    [
      __dirname + '/fixtures/node-4-space-indent.js',
      __dirname + '/fixtures/node-2-space-indent.js',
      __dirname + '/fixtures/node-4-space-indent.js'
    ],
    {
      stdout: '0 errors\n',
      stderr: '',
      exitCode: 0
    },
    test
  );
};
