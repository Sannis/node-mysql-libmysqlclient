#!/usr/bin/env node
/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Require modules
var
  assert = require("assert"),
  util = require("util"),
  fs = require("fs"),
  path = require("path");

var
  regex_class_source_filename =
    /mysql_bindings_.*\.h$/,
  regex_class_name =
    /class [a-z:\s]*?([a-z]*) :[a-z:\s]*\{/i,
  regex_class_methods =
    /static Handle<Value> ([a-z]*)\(const Arguments& args\);/ig,
  regex_class_properties_getters =
    /static Handle<Value> ([a-z]*Getter)\(Local<String> property/ig,
  regex_class_properties_setters =
    /static Handle<Value> ([a-z]*Setter)\(Local<String> property/ig,
  regex_method_is_sync =
    /^(.*Sync|New)$/;

var root_dir = path.normalize(__dirname + "/..");
var source_dir = path.normalize(root_dir + "/src");
var tests_dir = path.normalize(root_dir + "/tests");

function getBindingsClasses() {
  var
    source_files = fs.readdirSync(source_dir),
    file_content,
    class_name,
    classes = [],
    class_method_match,
    class_methods = [],
    class_property_getter_match,
    class_properties_getters = [],
    class_property_setter_match,
    class_properties_setters = [],
    i;

  for (i = 0; i < source_files.length; i += 1) {
    console.log("Parsing " + source_files[i] + "...");

    if (source_files[i].match(regex_class_source_filename)) {
      file_content = fs.readFileSync(source_dir + "/" + source_files[i]).toString();

      class_name = file_content.match(regex_class_name)[1];
      
      class_properties_getters = [];
      
      regex_class_properties_getters.lastIndex = 0;
      while ((class_property_getter_match = regex_class_properties_getters.exec(file_content))) {
        class_properties_getters.push(class_property_getter_match[1]);
        regex_class_properties_getters.lastIndex += 1;
      }
      
      class_properties_setters = [];
      
      regex_class_properties_setters.lastIndex = 0;
      while ((class_property_setter_match = regex_class_properties_setters.exec(file_content))) {
        class_properties_setters.push(class_property_setter_match[1]);
        regex_class_properties_setters.lastIndex += 1;
      }
      
      class_methods = [];
      
      regex_class_methods.lastIndex = 0;
      while ((class_method_match = regex_class_methods.exec(file_content))) {
        class_methods.push(class_method_match[1]);
        regex_class_methods.lastIndex += 1;
      }

      classes.push({
        name: class_name,
        properties_getters: class_properties_getters,
        properties_setters: class_properties_setters,
        methods: class_methods
      });
    }
  }
  
  classes.sort(function (a, b) {
    if (a.name < b.name) {
      return -1;
    }
    if (a.name > b.name) {
      return 1;
    }
    return 0;
  });
  
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
  test_file_name_sync = tests_dir + "/low-level-sync/test-class-" + bindings_classes[i].name.toLowerCase() + "-sync.js";

  console.log(bold(bindings_classes[i].name + ": " + path.relative(root_dir, test_file_name_sync)));

  try {
    test_require_sync = require(test_file_name_sync.replace(/\.js$/, ''));
  }
  catch (e) {
    test_require_sync = false;
  }
  
  for (j = 0; j < bindings_classes[i].properties_getters.length; j += 1) {
    if (test_require_sync
     && (typeof test_require_sync[bindings_classes[i].properties_getters[j]] !== 'undefined')
    ) {
      console.log(green('✔ ' + bindings_classes[i].properties_getters[j]));
    } else {
      console.log(red('✖ ' + bindings_classes[i].properties_getters[j]));
      notexist_tests += 1;
    }
  }
  
  for (j = 0; j < bindings_classes[i].properties_setters.length; j += 1) {
    if (test_require_sync
     && (typeof test_require_sync[bindings_classes[i].properties_setters[j]] !== 'undefined')
    ) {
      console.log(green('✔ ' + bindings_classes[i].properties_setters[j]));
    } else {
      console.log(red('✖ ' + bindings_classes[i].properties_setters[j]));
      notexist_tests += 1;
    }
  }
  
  for (j = 0; j < bindings_classes[i].methods.length; j += 1) {
    if (bindings_classes[i].methods[j].match(regex_method_is_sync)) {
      if (test_require_sync
       && (typeof test_require_sync[bindings_classes[i].methods[j]] !== 'undefined')
      ) {
        console.log(green('✔ ' + bindings_classes[i].methods[j]));
      } else {
        console.log(red('✖ ' + bindings_classes[i].methods[j]));
        notexist_tests += 1;
      }
    }
  }

  test_file_name = tests_dir + "/low-level-async/test-class-" + bindings_classes[i].name.toLowerCase() + "-async.js";

  console.log(bold(bindings_classes[i].name + ": " + path.relative(root_dir, test_file_name)));

  try {
    test_require = require(test_file_name.replace(/\.js$/, ''));
  }
  catch (e) {
    test_require = false;
  }

  for (j = 0; j < bindings_classes[i].methods.length; j += 1) {
    if (!bindings_classes[i].methods[j].match(regex_method_is_sync)) {
      if (test_require
       && (typeof test_require[bindings_classes[i].methods[j]] !== 'undefined')
      ) {
        console.log(green('✔ ' + bindings_classes[i].methods[j]));
      } else {
        console.log(red('✖ ' + bindings_classes[i].methods[j]));
        notexist_tests += 1;
      }
    }
  }
}

if (notexist_tests) {
  console.log('\n' + bold(red('NONEXISTENT METHODS TESTS: ')) + notexist_tests);
} else {
  console.log('\n' + bold(green('OK')));
}


