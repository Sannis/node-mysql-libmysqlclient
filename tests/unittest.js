/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

Code from Eric Florenzano's node-jsonrpc test.js
http://github.com/ericflo/node-jsonrpc/blob/master/test/test.js
http://www.eflorenzano.com/
Copyright (C) 2009, Eric Florenzano <floguy@gmail.com>

See license text in LICENSE file
*/

var sys = require('sys');

exports.unittest = {
  passed: 0,
  failed: 0,
  assertions: 0,

  test: function (description, block) {
    var _puts  = sys.puts,
      output = "",
      result = '?',
      _boom = null;
    sys.puts = function (s) {
      output += s + "\n";
    };
    try {
      sys.print("  " + description + " ...");
      block();
      result = '.';
    }
    catch (boom) {
      if (boom === 'FAIL') {
        result = 'F';
      } else {
        result = 'E';
        _boom = boom;
        sys.puts(boom.toString());
      }
    }
    sys.puts = _puts;
    if (result === '.') {
      sys.print(" OK\n");
      this.passed += 1;
    }
    else {
      sys.print(" FAIL\n");
      if (output !== "") {
        sys.print(output.replace(/^/, "    ") + "\n");
      }
      this.failed += 1;
      if (_boom) {
        throw _boom;
      }
    }
  },

  assert: function (value, description) {
    this.assertions += 1;
    if (description) {
      sys.puts("ASSERT: " + description);
    }
    if (!value) {
      throw 'FAIL';
    }
  },

  assertEqual: function (expect, is) {
    this.assert(
      expect === is,
      sys.inspect(expect) + " == " + sys.inspect(is)
    );
  },
  
  // This fumction comes from node/lib/assert.js
  // Originally from narwhal.js (http://narwhaljs.org)
  // Copyright (c) 2009 Thomas Robinson <280north.com>
  assertInspectEqual: function (expect, is) {
    this.assert(
      sys.inspect(expect) === sys.inspect(is),
      sys.inspect(expect) + " == " + sys.inspect(is)
    )
  },

  assertBoom: function (message, block) {
    var error = null;
    try {
      block();
    }
    catch (boom) {
      error = boom;
    }

    if (!error) {
      sys.puts('NO BOOM');
      throw 'FAIL';
    }
    if (error !== message) {
      sys.puts('BOOM: ' + sys.inspect(error) +
          ' [' + sys.inspect(message) + ' expected]');
      throw 'FAIL';
    }
  },

  showResults: function () {
    sys.puts("Tests: " + this.passed + " passed, " + this.failed + " failed");
  }
};

