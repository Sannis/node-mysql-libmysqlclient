#!/usr/bin/env node
/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Load configuration
var cfg = require("./config").cfg;

// Require modules
var
  sys = require("sys"),
  mysql_libmysqlclient = require("../mysql-libmysqlclient");


var conn = mysql_libmysqlclient.createConnection(cfg.host, cfg.user, cfg.password, cfg.database),
  res,
  random_number,
  random_boolean,
  last_insert_id,
  i;
  
var irc = cfg.insert_rows_count;
  
res = conn.query("DROP TABLE IF EXISTS " + cfg.test_table + ";");
res = conn.query("CREATE TABLE " + cfg.test_table +
  " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
  " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
  " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;");

if (!res) {
  sys.puts("conn.query('DELETE FROM cfg.test_table')");
  process.exit(1);
}

sys.puts("Start");

for (i = 0; i < irc; i += 1)
{
  random_number = Math.round(Math.random() * 1000000);
  random_boolean = (Math.random() > 0.5) ? 1 : 0;
  sys.puts("\033[1ABefore queryAsync #" + (i + 1));
  
  (function () {
    var j = i;
    conn.queryAsync("INSERT INTO " + cfg.test_table +
      " (random_number, random_boolean) VALUES ('" + random_number +
      "', '" + random_boolean + "');", function (result) {
        sys.puts("\033[1ACallback #" + (j + 1));
      });
  })();
}

sys.puts("Finish");

process.on('exit', function () {
  sys.puts("onExit");
  last_insert_id = conn.lastInsertId();
  if (last_insert_id !== irc) {
    sys.puts("\033[31mFAIL: " + last_insert_id + " !== " + irc + "\033[39m");
  } else {
    sys.puts("\033[32mOK: last_insert_id == irc\033[39m");
  }
  conn.close();
});

