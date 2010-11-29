/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require("../config").cfg;

// Require modules
var
  mysql_libmysqlclient = require("../../mysql-libmysqlclient"),
  mysql_bindings = require("../../mysql_bindings");

exports.FetchAll = function (test) {
  test.expect(5);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('1', '1');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                    " (random_number, random_boolean) VALUES ('2', '1');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('3', '0');") && res;
  test.ok(res, "INSERT");
  
  res = conn.querySync("SELECT random_number from " + cfg.test_table +
                   " WHERE random_boolean='0';");
  
  res.fetchAll(function (err, rows, fields) {
    test.ok(err === null, "res.fetchAll() err===null");
    
    test.same(rows, [{random_number: 3}], "conn.querySync('SELECT ...').fetchAll()");
    test.same(fields, res.fetchFieldsSync(), "Callback fields argument == res.fetchFieldsSync()");
    res.freeSync();
    conn.closeSync();
    
    test.done();
  });
};

