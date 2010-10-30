/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Load configuration
var cfg = require("../config").cfg;

// Require modules
var
  mysql_libmysqlclient = require("../../mysql-libmysqlclient");

exports.JoinQueryFetchAllSync = function (test) {
  test.expect(4);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    tables,
    rows;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('1', '1');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                    " (random_number, random_boolean) VALUES ('2', '1');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('3', '0');") && res;
  test.ok(res, "INSERT");
  
  res = conn.querySync("SELECT t1.*, t2.* from " + cfg.test_table + " t1 JOIN " + cfg.test_table + " t2 ON t1.random_boolean = t2.random_boolean;");
  test.ok(res, "SELECT with JOIN");
  rows = res.fetchAllSync();
  console.log(rows);
  test.same(rows, [ { "t1.random_number": 1, "t1.random_boolean": 1, "t2.random_number": 1, "t2.random_boolean": 1 },
    { "t1.random_number": 2, "t1.random_boolean": 1, "t2.random_number": 1, "t2.random_boolean": 1 },
    { "t1.random_number": 1, "t1.random_boolean": 1, "t2.random_number": 2, "t2.random_boolean": 1 },
    { "t1.random_number": 2, "t1.random_boolean": 1, "t2.random_number": 2, "t2.random_boolean": 1 },
    { "t1.random_number": 3, "t1.random_boolean": 0, "t2.random_number": 3, "t2.random_boolean": 0 }
    ], "conn.querySync('SELECT ...').fetchAllSync()");
  
  conn.closeSync();
  
  test.done();
};

