/** internal, section: Plugins
 *  Parsers.ndoc(NDoc) -> Void
 *
 *  Registers NDoc parser as `ndoc`.
 *
 *
 *  ##### Example
 *
 *      NDoc.parse(files, options);
 **/


'use strict';


// stdlib
var fs = require('fs');


// 3rd-party
var _ = require('lodash');


// internal
var parser = require('./javascript/parser');


////////////////////////////////////////////////////////////////////////////////


function parse_javascript(file /*, options*/) {
  var source = fs.readFileSync(file, 'utf8');

  var nodes, list = {};

  try {
    nodes = parser.parse(source);
  } catch (err) {
    throw err + ' [' + file + ':' + err.line + ',' + err.column + ']';
  }

  // do pre-distribute early work
  _.each(nodes, function (node /*, id*/) {
    var clone;

    // assign hierarchy helpers
    node.aliases  = [];
    node.children = [];

    // set source fiel of the node
    node.file = file;

    if ('class' === node.type) {
      node.subclasses = [];
    }

    // collect sections
    if ('section' === node.type) {
      list[node.id] = node;
      return;
    }

    // elements with undefined section get '' section,
    // and will be resolved later, when we'll have full
    // element list
    list[(node.section || '') + '.' + node.id] = node;

    // bound methods produce two methods with the same description but different signatures
    // E.g. Element.foo(@element, a, b) becomes
    // Element.foo(element, a, b) and Element#foo(a, b)
    if ('method' === node.type && node.bound) {
      clone = _.clone(node);
      clone.id = node.id.replace(/(.+)\.(.+)$/, '$1#$2');

      // link to methods
      node.bound = clone.id;
      clone.bound = node.id;

      // insert bound method clone
      list[(node.section || '') + '.' + clone.id] = clone;
    }
  });

  // TODO: section.related_to should mark related element as belonging to the section
  //_.each(list, function (node, id) {
  //  var ref_id = '.' + node.related_to, ref;
  //  if ('section' === node.type && node.related_to && list[ref_id]) {
  //    ref = list[ref_id];
  //    ref.id = node.id + '.' + node.related_to;
  //    delete list[ref_id];
  //    list[ref.id] = ref;
  //  }
  //});

  return list;
}


////////////////////////////////////////////////////////////////////////////////


module.exports = function (NDoc) {
  NDoc.registerParser('.js', parse_javascript);
};
