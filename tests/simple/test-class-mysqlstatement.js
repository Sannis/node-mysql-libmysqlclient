/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require('../config');

exports.New = function (test) {
  test.expect(1);
  
  test.throws(function () {
    var stmt = new cfg.mysql_bindings.MysqlStatement();
  }, TypeError, "new mysql_bindings.MysqlStatement() should throw exception from JS code");
  
  test.done();
};

exports.SendLongDataSync = function (test) {
  test.expect(11);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    stmt,
    res,
    rows,
    test_message1 = "MySQL -- The most popular Open Source database",
    test_message2 = "\n Second chunk of message";
  
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  test.ok(res, "conn.querySync('DELETE FROM cfg.test_table')");
  
  res = conn.querySync("ALTER TABLE " + cfg.test_table + " ADD message TEXT;");
  test.ok(res, "conn.querySync('ALTER TABLE test_table ADD message TEXT')");
  
  stmt = conn.initStatementSync();
  test.ok(stmt);
  
  res = stmt.prepareSync("INSERT INTO " + cfg.test_table + " (message) VALUES (?);");
  test.ok(res, "stmt.prepareSync('INSERT INTO test_table (message) VALUES (?)')");
  
  res = stmt.bindParamsSync([""]);
  test.ok(res, "stmt.bindParamSync([''])");
  
  test.ok(stmt.sendLongDataSync(0, test_message1), "stmt.sendLongDataSync(0, test_message1)");
  test.ok(stmt.sendLongDataSync(0, test_message2), "stmt.sendLongDataSync(0, test_message2)");
  
  res = stmt.executeSync();
  test.ok(res, "stmt.sendLongDataSync(0, test_message).executeSync()");
  
  res = conn.querySync("SELECT message from " + cfg.test_table + " WHERE message !='';");
  rows = res.fetchAllSync();
  test.same(rows, [{message: test_message1 + test_message2}], "conn.querySync('SELECT message ... ').fetchAllSync()");
  
  res = conn.querySync("ALTER TABLE " + cfg.test_table + " DROP message;");
  test.ok(res, "conn.querySync('ALTER TABLE test_table  DROP message')");
  
  conn.closeSync();
  
  test.done();
};

