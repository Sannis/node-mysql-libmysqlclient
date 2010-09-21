#!/usr/bin/env node
/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Require modules
var
  sys = require("sys"),
  stdin = process.openStdin(),
  node_gc = require("./node-gc/gc"),
  gc = new node_gc.GC();

var
  initial_mu;

var
  commands = {
    q: function () {
      process.exit(0);
    },
    s: function () {
      show_memory_usage();
    },
    gc: function () {
      gc.collect();
    }
  }

function show_memory_usage_line(title, value0, value1) {
  sys.puts(title + ": " + value1 + (value1 > value0 ? " (+" : " (") + (100*(value1 - value0)/value0).toFixed(2) + "%)");
}

function show_memory_usage() {
  var mu;
  
  if (!initial_mu) {
    initial_mu = process.memoryUsage();
    sys.puts("Initial memory usage: " + sys.inspect(initial_mu));
  } else {
    mu = process.memoryUsage();
    
    sys.puts("Currect memory usage:");
    show_memory_usage_line("rss", initial_mu.rss, mu.rss);
    show_memory_usage_line("vsize", initial_mu.vsize, mu.vsize);
    show_memory_usage_line("heapUsed", initial_mu.heapUsed, mu.heapUsed);
  }
}


process.on('exit', function () {
  show_memory_usage();
});

// Main program

sys.puts("Welcome to the memory leaks finder!");
gc.collect();
show_memory_usage();
sys.print("mlf> ");
stdin.setEncoding('ascii');
stdin.addListener("data", function(str) {
  str = str.trim();
  
  var pair = str.split(' ');
  
  pair[1] = parseInt(pair[1]) > 0 ? parseInt(pair[1]) : 1;
  
  if (commands[pair[0]]) {
    for (var i = 0; i < pair[1]; i += 1) {
      commands[pair[0]].apply();
    }
  } else {
    sys.puts("Unrecognized command: " + pair[0]);
  }
  
  sys.print("mlf> ");
});

