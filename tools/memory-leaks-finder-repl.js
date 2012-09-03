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
  helper = require("./memory-helper"),
// Load configuration
  cfg = require("../tests/config"),
// Params
  prompt = "mlf> ",
  commands;

commands = {
  quit: function () {
    process.exit(0);
  },
  showMemoryUsageDelta: function () {
    helper.showMemoryUsageDelta();
  },
  resetMemoryUsageDelta: function () {
    helper.resetMemoryUsageDelta();
  },
  gc: function () {
    helper.heavyGc();
  },
  help: function () {
    var cmd;
    process.stdout.write("List of commands:\n");
    for (cmd in commands) {
      if (commands.hasOwnProperty(cmd)) {
        process.stdout.write("  " + cmd + "\n");
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

    while ((row = res.fetchRowSync())) {
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

    while ((row = res.fetchRowSync())) {
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
    var conn = mysql.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);

    var res = conn.querySync("SELECT 'some string' AS str;");

    conn.closeSync();
  },

  manyQueries: function (n) {
    var conn = mysql.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);

    var res;

    for (var i = 0; i < n; i++) {
      res = conn.querySync("SELECT 'some string' AS str;");
    }
  }
};

commands.manyQueries.passTimesAsArgument = true;

// Main program
process.stdout.write("Starting WebKit development tools agent...\n");
process.stdout.write("Open http://c4milo.github.com/node-webkit-agent/19.0.1084.46/inspector.html?host=localhost:1337&page=0 to use it.\n");
require('webkit-devtools-agent');
process.emit('SIGUSR2');
process.stdout.write("\n");

process.stdout.write("Welcome to the memory leaks finder!\n");
process.stdout.write("Type 'help' for options.\n");

helper.heavyGc();
helper.showMemoryUsageDelta();

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
  process.stdout.write("\n");
  helper.showMemoryUsageDelta();
  process.stdin.destroy();
});

rli.on('line', function (cmd) {
  var pair = cmd.trim().split(/\s+/), i;

  pair[0] = pair[0].trim();
  pair[1] = parseInt(pair[1], 10) > 0 ? parseInt(pair[1], 10) : 1;

  if (commands[pair[0]]) {
    if (pair[0] === "help" || pair[0] === "quit" || pair[0] === "showMemoryUsage") {
      commands[pair[0]]();
    } else {
      try {
        process.stdout.write("Run " + pair[0] + " for " + pair[1] + " times:");

        if (commands[pair[0]].passTimesAsArgument) {
          commands[pair[0]](pair[1]);
        } else {
          for (i = 0; i < pair[1]; i += 1) {
            commands[pair[0]]();
          }
        }

        process.stdout.write(" done.\n");
      } catch (e) {
        process.stdout.write("Exception caused!\n");
        process.stdout.write(util.inspect(e.stack) + "\n");
      }

      helper.showMemoryUsageDelta();
    }
  } else if (pair[0] !== "") {
    process.stdout.write("Unrecognized command: " + pair[0] + "\n");
    commands.help();
  }

  rli.prompt();
});

rli.setPrompt(prompt);
rli.prompt();
