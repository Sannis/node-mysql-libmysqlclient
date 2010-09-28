#!/usr/bin/env node
/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

var
// Require modules
  sys = require("sys"),
  stdin,
  readline = require('readline'),
  rli,
  node_gc,
  gc,
  mysql_libmysqlclient = require("../mysql-libmysqlclient"),
  mysql_bindings = require("../mysql_bindings"),
  cfg = require("../tests/config").cfg,
// Params
  prompt = "mlf> ",
  commands,
// Initial memory usage
  initial_mu;

function show_memory_usage_line(title, value0, value1) {
  if (value1) {
    stdin.write(title + ": " + value1 + (value1 > value0 ? " (+" : " (") + (100 * (value1 - value0) / value0).toFixed(2) + "%)\n");
  } else {
    sys.puts(title + ": " + value0 + "\n");
  }
}

function show_memory_usage() {
  if (!initial_mu) {
    initial_mu = process.memoryUsage();
    
    sys.puts("Initial memory usage:");
    sys.puts("rss: " + initial_mu.rss);
    sys.puts("vsize: " + initial_mu.vsize);
    sys.puts("heapUsed: " + initial_mu.heapUsed);
  } else {
    var mu = process.memoryUsage();
    
    stdin.write("Currect memory usage:\n");
    show_memory_usage_line("rss", initial_mu.rss, mu.rss);
    show_memory_usage_line("vsize", initial_mu.vsize, mu.vsize);
    show_memory_usage_line("heapUsed", initial_mu.heapUsed, mu.heapUsed);
  }
}

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
    var cmd;
    stdin.write("List of commands:\n");
    for (cmd in commands) {
      if (commands.hasOwnProperty(cmd)) {
        stdin.write(cmd + "\n");
      }
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
    
    conn.closeSync();
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
  fetch_lowlevel: function () {
    var
      conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
      res,
      row;
    
    conn.realQuerySync("SHOW TABLES;");
    res = conn.storeResultSync();
    
    while ((row = res.fetchArraySync())) {}
    
    conn.closeSync();
  },
  fetch_lowlevel_and_free: function () {
    var
      conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
      res,
      row;
    
    conn.realQuerySync("SHOW TABLES;");
    res = conn.storeResultSync();
    
    while ((row = res.fetchArraySync())) {}
    
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
};

// Main program

try {
  node_gc = require("./node-gc/gc");
} catch (e) {
  sys.puts("\u001b[31mNode-GC doesn't exists or doesn't builded.\u001b[39m\n" +
           "You should run:\n$> git submodule update --init\n" +
           "$> cd ./tools/node-gc\n$> node-waf configure build");
  process.exit(1);
}

gc = new node_gc.GC();

sys.puts("Welcome to the memory leaks finder!");
sys.puts("Type 'help' for options.");
gc.collect();
show_memory_usage();

stdin = process.openStdin();
rli = readline.createInterface(stdin, function (text) {
  //return complete(text);
  var
    completions = [],
    completeOn,
    i;
  
  completeOn = text;
  for (i in commands) {
    if (i.match(new RegExp("^" + text))) {
      completions.push(i);
    }
  }
  
  return [completions, completeOn];
});

rli.on("SIGINT", function () {
  rli.close();
});

rli.addListener('close', function () {
  show_memory_usage();
  stdin.destroy();
});

rli.addListener('line', function (cmd) {
  var
    pair = cmd.trim().split(/\s+/),
    i;

  pair[0] = pair[0].trim();
  pair[1] = parseInt(pair[1], 10) > 0 ? parseInt(pair[1], 10) : 1;

  if (commands[pair[0]]) {
    try {
      for (i = 0; i < pair[1]; i += 1) {
        commands[pair[0]].apply();
      }
    } catch (e) {
      stdin.write("Exception caused!\n");
      stdin.write(sys.inspect(e.stack) + "\n");
    }
    if (pair[0] !== "help") {
      show_memory_usage();
    }
  } else if (pair[0] !== "") {
    stdin.write("Unrecognized command: " + pair[0] + "\n");
    commands.help();
  }

  rli.prompt();
});
stdin.addListener("data", function (chunk) {
  rli.write(chunk);
});
rli.setPrompt(prompt);
rli.prompt();

