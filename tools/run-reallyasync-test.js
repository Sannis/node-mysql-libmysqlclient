#!/usr/bin/env node
/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require("./config").cfg;

// Require modules
var
  sys = require("sys"),
  mysql_libmysqlclient = require("../mysql-libmysqlclient");


var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
  res,
  random_number,
  random_boolean,
  last_insert_id,
  i, ti = 0,
  func;
  
res = conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table + ";");
res = conn.querySync("CREATE TABLE " + cfg.test_table +
  " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
  " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
  " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;");

if (!res) {
  sys.puts("Error in conn.query('DELETE FROM cfg.test_table')");
  process.exit(1);
}

sys.puts("Start");

for (i = 0; i < cfg.insert_rows_count; i += 1) {
  random_number = Math.round(Math.random() * 1000000);
  random_boolean = (Math.random() > 0.5) ? 1 : 0;
  sys.puts("\u001B[1ABefore queryAsync #" + (i + 1));
  
  func = function () {
    var j = i;
    conn.query("INSERT INTO " + cfg.test_table +
      " (random_number, random_boolean) VALUES ('" + random_number +
      "', '" + random_boolean + "');", function (err, result) {
        sys.puts("\u001B[1ACallback #" + (j + 1));
        if (result !== null) {
          result.freeSync();
        }
        ti += 1;
      });
  };
  
  func();
}

sys.puts("Finish");

process.on('exit', function () {
  sys.puts("onExit callbacks done: " + ti);
  last_insert_id = conn.lastInsertIdSync();
  if (last_insert_id !== cfg.insert_rows_count) {
    sys.puts("\u001B[31mFAIL: " + last_insert_id + " !== " + cfg.insert_rows_count + "\u001B[39m");
  } else {
    sys.puts("\u001B[32mOK: last_insert_id == cfg.insert_rows_count\u001B[39m");
  }
  conn.closeSync();
});

