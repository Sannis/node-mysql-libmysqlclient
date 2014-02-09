/*
 * Nodelint XML reporter
 *
 * This reporter produces XML that can be automatically recognised by
 * Hudson Violations Plugin http://wiki.hudson-ci.org/display/HUDSON/Violations+Plugin
 *
 * Released into the Public Domain by tav <tav@espians.com>
 * See the README.md for full credits of the awesome contributors!
 */

/**
 * Module dependencies
 */
var
  path = require('path'),
  util = require('util');

/**
 * Reporter info string
 */
exports.info = "XML reporter";

function escape(str) {
  return (str) ? str
                    .replace(/&/g, '&amp;')
                    .replace(/</g, '&lt;')
                    .replace(/>/g, '&gt;')
                    .replace(/"/g, '&quot;')
               : '';
}

/**
 * Report linting results to the command-line.
 *
 * @api public
 *
 * @param {Array} results
 */
exports.report = function report(results) {
  var
    i, error, len, file,
    dir = process.cwd(),
    xml = '<?xml version="1.0" encoding="UTF-8" ?>\n<jslint>\n';

  for (i = 0, len = results.length; i < len; i += 1) {
    file = path.resolve(dir, results[i].file);
    error = results[i].error;
    xml += '\t<file name="' + file + '">\n' +
           '\t\t<issue char="' + error.character + '" evidence="' + escape(error.evidence || '') +
           '" line="' + error.line + '" reason="' + escape(error.reason) + '"/>\n' +
           '\t</file>\n';
  }

  xml += '</jslint>';

  util.puts(xml);
};
