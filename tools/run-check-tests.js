#!/usr/bin/env node
/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

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
    /class [a-z:\s]*?([a-z]*) :[a-z:\s]*\{/i,
  regex_class_methods =
    /static Handle<Value> ([a-z]*)\(const Arguments& args\);/ig,
  regex_class_properties_getters =
    /static Handle<Value> ([a-z]*Getter)\(Local<String> property/ig,
  regex_class_properties_setters =
    /static Handle<Value> ([a-z]*Setter)\(Local<String> property/ig;

var tools_dir = fs.realpathSync(__dirname);
var source_dir = fs.realpathSync(tools_dir + "/../src");
var tests_dir = fs.realpathSync(tools_dir + "/../tests");

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
    if (source_files[i].match(/\.h$/)) {
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

      classes.push({name: class_name,
                    properties_getters: class_properties_getters,
                    properties_setters: class_properties_setters,
                    methods: class_methods});
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
sys.debug(sys.inspect(bindings_classes));
for (i = 0; i < bindings_classes.length; i += 1) {
  sys.puts(bold(bindings_classes[i].name + ": test-1-class-" + bindings_classes[i].name.toLowerCase() + ".js"));
  
  test_file_name = tests_dir + "/test-1-class-" + bindings_classes[i].name.toLowerCase() + ".js";
  
  try {
    test_require = require(test_file_name.replace(/\.js$/, ''));
  }
  catch (e) {
    test_require = false;
  }
  
  for (j = 0; j < bindings_classes[i].properties_getters.length; j += 1) {
    if (test_require && (typeof test_require[bindings_classes[i].properties_getters[j]] !== 'undefined')) {
      sys.puts(green('✔ ' + bindings_classes[i].properties_getters[j]));
    } else {
      sys.puts(red('✖ ' + bindings_classes[i].properties_getters[j]));
      notexist_tests += 1;
    }
  }
  
  for (j = 0; j < bindings_classes[i].properties_setters.length; j += 1) {
    if (test_require && (typeof test_require[bindings_classes[i].properties_setters[j]] !== 'undefined')) {
      sys.puts(green('✔ ' + bindings_classes[i].properties_setters[j]));
    } else {
      sys.puts(red('✖ ' + bindings_classes[i].properties_setters[j]));
      notexist_tests += 1;
    }
  }
  
  for (j = 0; j < bindings_classes[i].methods.length; j += 1) {
    if (test_require && (typeof test_require[bindings_classes[i].methods[j]] !== 'undefined')) {
      sys.puts(green('✔ ' + bindings_classes[i].methods[j]));
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


