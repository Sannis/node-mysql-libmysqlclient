/*
 * Nodelint default reporter
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
exports.info = "Default reporter";

/**
 * Should we use ANSI coloring?
 * Credits Coffeescript Cakefile
 */
var enableColors = false;
if (process.platform !== 'win32') {
  enableColors = (process.env.NODE_DISABLE_COLORS === "1") ? false : true;
}

/**
 * Customise the error reporting -- the following colours the text red if enableColors
 */
var
  errorPrefix = enableColors ? "\u001b[1;31m" : "",
  errorSuffix = enableColors ? "\u001b[0m"    : "";

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
    error;

  for (i = 0; i < len; i += 1) {
    error = results[i].error;
    str += errorPrefix + results[i].file  + ', line ' + error.line +
           ', character ' + error.character + ":" + errorSuffix + " " +
            error.reason + '\n' +
            (error.evidence || '').replace(error_regexp, "$1") + '\n';
  }

  str += len + ' error' + ((len === 1) ? '' : 's');

  util.puts(str);
};
