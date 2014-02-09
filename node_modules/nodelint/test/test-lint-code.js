/*
 * Nodelint code style test
 *
 * Released into the Public Domain by tav <tav@espians.com>
 * See the README.md for full credits of the awesome contributors!
 */

/**
 * Module dependencies
 */
var helper = require('./helper');

/**
 * Lint nodelint own code
 *
 * @param test
 */
exports.MakeLint = function (test) {
  helper.testConsoleOutput(
    './nodelint',
    ['./nodelint', './config.js', './lib/', './test/'],
    {
      stdout: '0 errors\n',
      stderr: '',
      exitCode: 0
    },
    test
  );
};
