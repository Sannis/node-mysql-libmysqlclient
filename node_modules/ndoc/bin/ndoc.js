#!/usr/bin/env node


'use strict';


// stdlib
var fs = require('fs');
var path = require('path');


// internal
var NDoc      = require('..');
var template  = require('../lib/ndoc/common').template;


////////////////////////////////////////////////////////////////////////////////


function exit(err) {
  if (err) {
    console.error(err.message || err);
    process.exit(1);
  }

  process.exit(0);
}


////////////////////////////////////////////////////////////////////////////////


NDoc.cli.addArgument(['--noenv'], {
  help:   'Ignore .ndocrc',
  action: 'storeTrue'
});


if (-1 === process.argv.indexOf('--noenv') && fs.existsSync('.ndocrc')) {
  NDoc.cli.readEnvFile('.ndocrc');
}


//
// preprocess plugins
//


NDoc.cli.parseKnownArgs().shift().use.forEach(function (pathname) {
  if (/^\./.test(pathname)) {
    pathname = path.resolve(process.cwd(), pathname);
  }

  try {
    NDoc.use(require(pathname));
  } catch (err) {
    exit('Failed add renderer: ' + pathname + '\n\n' + err.toString());
  }
});


//
// parse options
//


var options = NDoc.cli.parseArgs();


//
// Process aliases
//

options.aliases.forEach(function (pair) {
  NDoc.extensionAlias.apply(null, pair.split(':'));
});

//
// Post-process some of the options
//

options.title = template(options.title || '', {'package': options.package});
options.index = options.index || '';

//
// collect sources, parse into ast, render
//
try {
  var files = NDoc.cli.findFiles(options.paths, options.exclude);
  var ast = NDoc.parse(files, options);
  NDoc.render(options.renderer, ast, options);
} catch (e) {
  exit (e);
}
