/*
 * Nodelint reporters tests
 *
 * Released into the Public Domain by tav <tav@espians.com>
 * See the README.md for full credits of the awesome contributors!
 */

/**
 * Module dependencies
 */
var helper = require('./helper');

/**
 * Test for default reporter
 *
 * @param test
 */
exports.Default = function (test) {
  helper.testConsoleOutput(
    './nodelint',
    ['--reporter=default', __dirname + '/fixtures/no-errors.js'],
    {
      stdout: '0 errors\n',
      stderr: '',
      exitCode: 0
    },
    test
  );
};

/**
 * Test for IDEA reporter
 *
 * @param test
 */
exports.IDEA = function (test) {
  helper.testConsoleOutput(
    './nodelint',
    ['--reporter=idea', __dirname + '/fixtures/no-errors.js'],
    {
      stdout: '',
      stderr: '',
      exitCode: 0
    },
    test
  );
};

/**
 * Test for Textmate full HTML reporter
 *
 * @param test
 */
exports.TextmateFull = function (test) {
  var expectedStdout = '<html><head><style type="text/css">'
                     + 'body {font-size: 14px;}'
                     + 'pre { background-color: #eee; color: #400; margin: 3px 0;}'
                     + 'h1, h2 { font-family:"Arial, Helvetica"; margin: 0 0 5px; }'
                     + 'h1 { font-size: 20px; }'
                     + 'h2 { font-size: 16px;}a { font-family:"Arial, Helvetica";}'
                     + 'ul { margin: 10px 0 0 20px; padding: 0; list-style: none;}'
                     + 'li { margin: 0 0 10px; }'
                     + '</style></head><body><h1>0 Errors</h1><hr/><ul></ul></body></html>\n';
  helper.testConsoleOutput(
    './nodelint',
    ['--reporter=textmate_full', __dirname + '/fixtures/no-errors.js'],
    {
      stdout: expectedStdout,
      stderr: '',
      exitCode: 0
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
  helper.testConsoleOutput(
    './nodelint',
    ['--reporter=textmate_summary', __dirname + '/fixtures/no-errors.js'],
    {
      stdout: '',
      stderr: '',
      exitCode: 0
    },
    test
  );
};

/**
 * Test for VIM reporter
 *
 * @param test
 */
exports.VIM = function (test) {
  helper.testConsoleOutput(
    './nodelint',
    ['--reporter=vim', __dirname + '/fixtures/no-errors.js'],
    {
      stdout: '',
      stderr: '',
      exitCode: 0
    },
    test
  );
};

/**
 * Test for XML reporter
 *
 * @param test
 */
exports.XML = function (test) {
  helper.testConsoleOutput(
    './nodelint',
    ['--reporter=xml', __dirname + '/fixtures/no-errors.js'],
    {
      stdout: '<?xml version="1.0" encoding="UTF-8" ?>\n<jslint>\n</jslint>\n',
      stderr: '',
      exitCode: 0
    },
    test
  );
};
