/*
 * Nodelint IDEA reporter
 *
 * Reporter for working with external tools within the Idea IDEs
 * Only tested with RubyMine
 * For setup instructions see: https://github.com/tav/nodelint/wiki/Editor-and-IDE-integration
 *
 * Released into the Public Domain by tav <tav@espians.com>
 * See the README.md for full credits of the awesome contributors!
 */

/**
 * Module dependencies
 */
var util = require('util');

/**
 * Reporter info string
 */
exports.info = "IDEA reporter";

/**
 * Report linting results to the command-line.
 *
 * @api public
 *
 * @param {Array} results
 */
exports.report = function report(results) {
  var
    error_regexp = /^\s*(\S*(\s+\S+)*)\s*$/,
    i,
    len = results.length,
    str = '',
    file,
    error;

  if (len > 0) {
    for (i = 0; i < len; i += 1) {
      file = results[i].file;
      error = results[i].error;

      str += file  + ' ' + error.line +
        ':' + error.character +
        ' ' + error.reason + ' ' +
        (error.evidence || '').replace(error_regexp, "$1");
    }
    util.puts(str);
  }
};
