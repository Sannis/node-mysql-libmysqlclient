/*
 * Nodelint tests for JS file with Lint errors
 *
 * Released into the Public Domain by tav <tav@espians.com>
 * See the README.md for full credits of the awesome contributors!
 */

/**
 * Module dependencies
 */
var helper = require('./helper');

/**
 * Test for default reporter, no coloring
 *
 * @param test
 */
exports.DefaultNoColor = function (test) {
  var env = process.env;
  env.NODE_DISABLE_COLORS = 1;

  helper.testConsoleOutput(
    './nodelint',
    ['--reporter=default', __dirname + '/fixtures/some-errors.js.nolint'],
    {env: env},
    {
      stdout: __dirname + "/fixtures/some-errors.js.nolint, line 5, character 7: "
            + "Expected a conditional expression and instead saw an assignment.\n"
            + "if (a = b) {\n1 error\n",
      stderr: '',
      exitCode: 2
    },
    test
  );
};

/**
 * Test for default reporter, coloring
 *
 * @param test
 */
exports.DefaultColor = function (test) {
  var env = process.env;
  env.NODE_DISABLE_COLORS = 0;

  helper.testConsoleOutput(
    './nodelint',
    ['--reporter=default', __dirname + '/fixtures/some-errors.js.nolint'],
    {env: env},
    {
      stdout: "\u001b[1;31m"
            + __dirname + "/fixtures/some-errors.js.nolint, line 5, character 7:\u001b[0m "
            + "Expected a conditional expression and instead saw an assignment.\n"
            + "if (a = b) {\n1 error\n",
      stderr: '',
      exitCode: 2
    },
    test
  );
};

/**
 * Test for Textmate summary reporter
 *
 * @param test
 */
exports.TextmateSummary = function (test) {
  var env = process.env;
  env.NODE_DISABLE_COLORS = 1;

  helper.testConsoleOutput(
    './nodelint',
    ['--reporter=textmate_summary', __dirname + '/fixtures/some-errors.js.nolint'],
    {env: env},
    {
      stdout: "some-errors.js.nolint: line 5, character 7, "
            + "Expected a conditional expression and instead saw an assignment.\n"
            + "if (a = b) {\n\n1 error\n",
      stderr: '',
      exitCode: 2
    },
    test
  );
};
