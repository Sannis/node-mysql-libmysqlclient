/*
 * Nodelint tests helper
 *
 * Released into the Public Domain by tav <tav@espians.com>
 * See the README.md for full credits of the awesome contributors!
 */

/**
 * Module dependencies
 */
var childProcess = require('child_process');

var node_deprecated_warning = "The \"sys\" module is now called \"util\". "
                            + "It should have a similar interface.\n";

/**
 * Helper for console run output test
 */
function testConsoleOutput(file, args, options, expected, test) {
  if (typeof test === 'undefined') {
    test = expected;
    expected = options;
    options = null;
  }

  childProcess.execFile(file, args, options, function (error, stdout, stderr) {
    var testsCount = 0;
    if (expected.hasOwnProperty('exitCode')) {
      testsCount += 1; // error.code || !error
    }
    if (expected.hasOwnProperty('exitSignal')) {
      testsCount += 1; // error
      testsCount += 1; // error.signal
    }
    if (expected.hasOwnProperty('stdout')) {
      testsCount += 1; // stdout
    }
    if (expected.hasOwnProperty('stderr')) {
      testsCount += 1; // stderr
    }

    test.expect(testsCount);

    if (expected.hasOwnProperty('exitCode')) {
      if (expected.exitCode === 0) {
        test.equals(error, null);
      } else {
        test.equals(error.code, expected.exitCode);
      }
    }
    if (expected.hasOwnProperty('exitSignal')) {
      test.ok(error instanceof Error);
      test.equals(error.signal, expected.exitSignal);
    }
    if (expected.hasOwnProperty('stdout')) {
      test.equals(stdout.replace(node_deprecated_warning, ''), expected.stdout);
    }
    if (expected.hasOwnProperty('stderr')) {
      test.equals(stderr.replace(node_deprecated_warning, ''), expected.stderr);
    }

    test.done();
  });
}

exports.testConsoleOutput = testConsoleOutput;
