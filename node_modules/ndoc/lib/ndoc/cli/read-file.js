'use strict';


// stdlib
var fs = require('fs');


// 3rd-party
var Action = require('argparse').Action;


////////////////////////////////////////////////////////////////////////////////


var ReadFile = module.exports = function ReadFile(options) {
  options = options || {};
  options.nargs = 1;

  Action.call(this, options);
};


require('util').inherits(ReadFile, Action);


ReadFile.prototype.call = function (parser, namespace, values) {
  namespace.set(this.dest, fs.readFileSync(values.shift(), 'utf8'));
};
