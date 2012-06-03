#!/usr/bin/env node --expose-gc
/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

var
// Require modules
  util = require("util"),
  readline = require('readline'),
  rli,
  mysql = require("../"),
  mysql_bindings = mysql.bindings,
  cfg = require("../tests/config"),
// Params
  prompt = "mlf> ",
  commands,
// Initial memory usage
  initial_mu;

function show_memory_usage_line(title, value0, value1) {
  if (value1) {
    process.stdin.write(title + ": " + value1 + (value1 > value0 ? " (+" : " (") + (100 * (value1 - value0) / value0).toFixed(2) + "%)\n");
  } else {
    process.stdin.write(title + ": " + value0 + "\n");
  }
}

function show_memory_usage() {
  if (!initial_mu) {
    initial_mu = process.memoryUsage();
    
    process.stdin.write("Initial memory usage:\n");
    process.stdin.write("rss: " + initial_mu.rss + "\n");
    process.stdin.write("heapUsed: " + initial_mu.heapUsed + "\n");
    process.stdin.write("heapTotal: " + initial_mu.heapTotal + "\n");
  } else {
    var mu = process.memoryUsage();
    
    process.stdin.write("Currect memory usage:\n");
    show_memory_usage_line("rss", initial_mu.rss, mu.rss);
    show_memory_usage_line("heapUsed", initial_mu.heapUsed, mu.heapUsed);
    show_memory_usage_line("heapTotal", initial_mu.heapTotal, mu.heapTotal);
  }
}

commands = {
  quit: function () {
    process.exit(0);
  },
  usage: function () {
    show_memory_usage();
  },
  gc: function () {
    gc();
  },
  help: function () {
    var cmd;
    process.stdin.write("List of commands:\n");
    for (cmd in commands) {
      if (commands.hasOwnProperty(cmd)) {
        process.stdin.write(cmd + "\n");
      }
    }
  },

  newConnection: function () {
    var conn = new mysql_bindings.MysqlConnection();
  },
  createConnectionSync: function () {
    var conn = mysql.createConnectionSync(cfg.host, cfg.user, cfg.password);
  },
  createConnectionQueuedSync: function () {
    var conn = mysql.createConnectionQueuedSync(cfg.host, cfg.user, cfg.password);
  },
  createConnectionHighlevelSync: function () {
    var conn = mysql.createConnectionHighlevelSync(cfg.host, cfg.user, cfg.password);
  },

  errorOnConnect: function () {
    var conn = mysql.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database_denied);

    var connectionError = conn.connectionError;
  },
  errorOnQuery: function () {
    var conn = mysql.createConnectionSync(cfg.host, cfg.user, cfg.password);

    conn.querySync("USE " + cfg.database_denied + ";");

    var queryError = conn.errorSync();

    conn.closeSync();
  },

  fetchAllSync: function () {
    var
      conn = mysql.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
      res,
      rows;

    res = conn.querySync("SELECT 'some string' as str;");
    rows = res.fetchAllSync();

    conn.closeSync();
  },
  fetchAllSyncAndFree: function () {
    var
      conn = mysql.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
      res,
      rows;

    res = conn.querySync("SELECT 'some string' as str;");
    rows = res.fetchAllSync();

    res.freeSync();
    conn.closeSync();
  },
  fetchAllByRows: function () {
    var
      conn = mysql.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
      res,
      row;

    conn.realQuerySync("SHOW TABLES;");
    res = conn.storeResultSync();

    while ((row = res.fetchArraySync())) {
      // Empty block
    }

    conn.closeSync();
  },
  fetchAllByRowsAndFree: function () {
    var
      conn = mysql.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
      res,
      row;

    conn.realQuerySync("SHOW TABLES;");
    res = conn.storeResultSync();

    while ((row = res.fetchArraySync())) {
      // Empty block
    }

    res.freeSync();
    conn.closeSync();
  },

  escapeSync: function () {
    var
      conn = mysql.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
      str;

    str = conn.escapeSync("some string");

    conn.closeSync();
  },

  testResultsFreeAfterConnectionClose: function () {
    var
      conn = mysql.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
      str;
    
    res = conn.querySync("SELECT 'some string' as str;");

    conn.closeSync();
  }
};

// Main program
process.stdin.write("Starting WebKit development tools agent...\n");
process.stdin.write("Open http://c4milo.github.com/node-webkit-agent/19.0.1084.46/inspector.html?host=localhost:1337&page=0 to use it.\n");
require('webkit-devtools-agent');
process.emit('SIGUSR2');
process.stdin.write("\n");

process.stdin.write("Welcome to the memory leaks finder!\n");
process.stdin.write("Type 'help' for options.\n");

gc();
show_memory_usage();

rli = readline.createInterface(process.stdin, process.stdout, function completer (text) {
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

rli.on('close', function () {
  show_memory_usage();
  process.stdin.destroy();
});

rli.on('line', function (cmd) {
  var
    pair = cmd.trim().split(/\s+/),
    i;

  pair[0] = pair[0].trim();
  pair[1] = parseInt(pair[1], 10) > 0 ? parseInt(pair[1], 10) : 1;

  if (commands[pair[0]]) {
    try {
      process.stdin.write("Run " + pair[0] + " for " + pair[1] + " times:");
      for (i = 0; i < pair[1]; i += 1) {
        commands[pair[0]].apply();
      }
      process.stdin.write(" done.\n");
    } catch (e) {
      process.stdin.write("Exception caused!\n");
      process.stdin.write(util.inspect(e.stack) + "\n");
    }
    if (pair[0] !== "help") {
      show_memory_usage();
    }
  } else if (pair[0] !== "") {
    process.stdin.write("Unrecognized command: " + pair[0] + "\n");
    commands.help();
  }

  rli.prompt();
});

rli.setPrompt(prompt);
rli.prompt();
