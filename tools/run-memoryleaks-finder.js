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
  gc = new node_gc.GC(),
  mysql_libmysqlclient = require("../mysql-libmysqlclient"),
  mysql_bindings = require("../mysql_bindings"),
  cfg = require("./config").cfg;

var
  initial_mu;

var
  commands = {
    quit: function () {
      process.exit(0);
    },
    show_memory_usage: function () {
      show_memory_usage();
    },
    gc: function () {
      gc.collect();
    },
    help: function () {
      sys.puts("List of commands:");
      for (var i in commands) {
        sys.puts(i);
      }
    },
    new_connection: function () {
      var conn = new mysql_bindings.MysqlConnection();
    },
    error_in_connect: function () {
      var
        conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database_denied),
        error = conn.connectionError;
    },
    error_in_query: function () {
      var
        conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password),
        error;
      
      conn.querySync("USE " + cfg.database_denied + ";");
      error = conn.errorSync();
    },
    fetch_all: function () {
      var
        conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
        res,
        rows;
      
      res = conn.querySync("SELECT 'some string' as str;");
      rows = res.fetchAllSync();
      conn.closeSync();
    },
    fetch_all_and_free: function () {
      var
        conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
        res,
        rows;
      
      res = conn.querySync("SELECT 'some string' as str;");
      rows = res.fetchAllSync();
      res.freeSync();
      conn.closeSync();
    },
    escape: function () {
      var
        conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
        str;
      
      str = conn.escapeSync("some string");
      conn.closeSync();
    }
  }

function show_memory_usage_line(title, value0, value1) {
  sys.puts(title + ": " + value1 + (value1 > value0 ? " (+" : " (") + (100*(value1 - value0)/value0).toFixed(2) + "%)");
}

function show_memory_usage() {
  var mu;
  
  if (!initial_mu) {
    initial_mu = process.memoryUsage();
    sys.puts("Initial memory usage:");
    sys.puts(sys.inspect(initial_mu));
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
sys.puts("Type 'help' for options.");
gc.collect();
show_memory_usage();
sys.print("mlf> ");
stdin.setEncoding('ascii');
stdin.addListener("data", function(str) {
  str = str.trim();
  
  var pair = str.split(' ');
  
  pair[1] = parseInt(pair[1]) > 0 ? parseInt(pair[1]) : 1;
  
  if (commands[pair[0]]) {
    try {
        for (var i = 0; i < pair[1]; i += 1) {
          commands[pair[0]].apply();
        }
    } catch (e) {
      sys.puts("Exception caused!");
      sys.puts(sys.inspect(e.stack));
    }
    show_memory_usage();
  } else {
    sys.puts("Unrecognized command: " + pair[0]);
  }
  
  sys.print("mlf> ");
});

