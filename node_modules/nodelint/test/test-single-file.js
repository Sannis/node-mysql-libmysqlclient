/*
 * Nodelint tests for single file linting
 *
 * Released into the Public Domain by tav <tav@espians.com>
 * See the README.md for full credits of the awesome contributors!
 */

/**
 * Module dependencies
 */
var helper = require('./helper');

/**
 * Test for 2-space indent file
 *
 * @param test
 */
exports.Node2SpaceIndent = function (test) {
  helper.testConsoleOutput(
    './nodelint',
    [__dirname + '/fixtures/node-2-space-indent.js'],
    {
      stdout: '0 errors\n',
      stderr: '',
      exitCode: 0
    },
    test
  );
};

/**
 * Test for s-space indent file
 *
 * @param test
 */
exports.Node4SpaceIndent = function (test) {
  helper.testConsoleOutput(
    './nodelint',
    [__dirname + '/fixtures/node-4-space-indent.js'],
    {
      stdout: '0 errors\n',
      stderr: '',
      exitCode: 0
    },
    test
  );
};


/**
 * Test for 4-space indent file with browser js
 *
 * @param test
 */
exports.Browser4SpaceIndent = function (test) {
  helper.testConsoleOutput(
    './nodelint',
    [__dirname + '/fixtures/browser-4-space-indent.js'],
    {
      stdout: '0 errors\n',
      stderr: '',
      exitCode: 0
    },
    test
  );
};
