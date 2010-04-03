#!/usr/bin/env node
/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Require modules
var
  assert = require("assert"),
  sys = require("sys"),
  fs = require("fs"),
  path = require("path");

var
  regex_class_name =
    /class .*?([a-z]*) :.*\{/i,
  regex_class_methods =
    /static Handle<Value> ([a-z]*)\(const Arguments& args\);/ig;

var tests_dir = fs.realpathSync(__dirname);
var source_dir = fs.realpathSync(tests_dir + "/../src");

function getBindingsClasses() {
  var
    source_files = fs.readdirSync(source_dir),
    file_content,
    class_name,
    classes = [],
    class_method_match,
    class_methods = [],
    i;

  for (i = 0; i < source_files.length; i += 1) {
    if (source_files[i].match(/\.h$/)) {
      file_content = fs.readFileSync(source_dir + "/" + source_files[i]);
      
      class_name = file_content.match(regex_class_name)[1];
      
      class_methods = [];
      
      regex_class_methods.lastIndex = 0;
      while ((class_method_match = regex_class_methods.exec(file_content))) {
        class_methods.push(class_method_match[1]);
        regex_class_methods.lastIndex += 1;
      }

      classes.push({name: class_name, methods: class_methods});
    }
  }
  
  return classes;
}

var red   = function (str) {
  return "\u001b[31m" + str + "\u001b[39m";
};

var green = function (str) {
  return "\u001b[32m" + str + "\u001b[39m";
};

var bold  = function (str) {
  return "\u001b[1m" + str + "\u001b[22m";
};

var
  bindings_classes = getBindingsClasses(),
  i, j,
  test_file_name,
  test_require,
  notexist_tests = 0;

for (i = 0; i < bindings_classes.length; i += 1) {
  sys.puts(bindings_classes[i].name + ": test-class-" + bindings_classes[i].name.toLowerCase() + ".js");
  
  test_file_name = tests_dir + "/test-class-" + bindings_classes[i].name.toLowerCase() + ".js";
  
  try {
    test_require = require(test_file_name.replace(/\.js$/, ''));
  }
  catch (e) {
    test_require = false;
  }
  
  for (j = 0; j < bindings_classes[i].methods.length; j += 1) {
    if (test_require && (typeof test_require[bindings_classes[i].methods[j]] !== 'undefined')) {
      sys.puts('✔ ' + bindings_classes[i].methods[j]);
    } else {
      sys.puts(red('✖ ' + bindings_classes[i].methods[j]));
      notexist_tests += 1;
    }
  }
}

if (notexist_tests) {
  sys.puts('\n' + bold(red('NONEXISTENT METHODS TESTS: ')) + notexist_tests);
} else {
  sys.puts('\n' + bold(green('OK')));
}


