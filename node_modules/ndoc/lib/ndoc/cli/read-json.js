'use strict';


// stdlib
var fs = require('fs');


// 3rd-party
var Action = require('argparse').Action;


////////////////////////////////////////////////////////////////////////////////


var ReadJSON = module.exports = function ReadJSON(options) {
  options = options || {};
  options.nargs = 1;

  Action.call(this, options);
};


require('util').inherits(ReadJSON, Action);


ReadJSON.prototype.call = function (parser, namespace, values) {
  var str = fs.readFileSync(values.shift(), 'utf8');
  namespace.set(this.dest, JSON.parse(str));
};
