#!/usr/bin/env node
/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Database connection parameters
var host = "localhost",
  user = "test",
  password = "",
  database = "test",
  database_denied = "mysql",
  test_table = "test_table",
  charset = "utf8",

  // Operations count for continuous tests
  reconnect_count = 10000,
  insert_rows_count = 10000,

  // Require modules
  sys = require("sys"),
  mysql_sync = require("../mysql-sync"),
  unittest = require("./unittest").unittest,

  conn,
  flag;

sys.print("Running tests for NodeJS syncronous MySQL binding...\n");

unittest.test('mysql_sync.createConnection()', function () {
  var conn = mysql_sync.createConnection(host, user, password, database);
  
  unittest.assert(conn);
  
  conn.close();
});

/*
// TODO: how to write this test?
unittest.test('conn.close()', function() {
  var conn = mysql_sync.createConnection(host, user, password, database);
  conn.close();
  unittest.assert(conn);
});
*/

unittest.test('conn.connect() without database selection', function () {
  var conn = mysql_sync.createConnection(host, user, password, database);
  
  conn.close();
  unittest.assert(conn.connect(host, user, password));
  
  conn.close();
});

unittest.test('conn.connect() for allowed database', function () {
  var conn = mysql_sync.createConnection(host, user, password, database);
  
  conn.close();
  unittest.assert(conn.connect(host, user, password, database));
  
  conn.close();
});

unittest.test('conn.connect() for denied database', function () {
  var conn = mysql_sync.createConnection(host, user, password, database);
  conn.close();
  
  unittest.assert(!conn.connect(host, user, password, database_denied));
});

unittest.test('conn.selectDb() for allowed database', function () {
  var conn = mysql_sync.createConnection(host, user, password);
  
  unittest.assert(conn.selectDb(database));
  
  conn.close();
});

unittest.test('conn.selectDb() for denied database', function () {
  var conn = mysql_sync.createConnection(host, user, password);
  
  unittest.assert(!conn.selectDb(database_denied));
  
  conn.close();
});

unittest.test('conn.setCharset()', function () {
  var conn = mysql_sync.createConnection(host, user, password);
  
  unittest.assert(conn.setCharset(charset));
  
  conn.close();
});

unittest.test('conn.connectErrno()', function () {
  var conn = mysql_sync.createConnection(host, user, password, database);
  
  conn.close();
  
  conn.connect(host, user, password, database_denied);
  
  unittest.assertEqual(1044, conn.connectErrno());
});

unittest.test('conn.connectError()', function () {
  var conn = mysql_sync.createConnection(host, user, password, database);
  
  conn.close();
  
  conn.connect(host, user, password, database_denied);
  
  unittest.assertEqual("Access denied for user ''@'" + host + "' to database '" + database_denied + "'", conn.connectError());
});

unittest.test('conn.getInfo()', function () {
  var conn = mysql_sync.createConnection(host, user, password, database),
    info = conn.getInfo(),
    i;
  
  unittest.assert(info);
  for (i in info) {
    if (typeof i === 'string') {
      sys.puts("info['" + i + "'] = " + info[i]);
    }
  }
  
  conn.close();
});

unittest.test('conn.escape()', function () {
  var conn = mysql_sync.createConnection(host, user, password, database),
    strings_to_escape = [
    ["test string", "test string"],
    ["test\\string", "test\\\\string"],
    ["test\nstring", "test\\nstring"],
    ["test\rstring", "test\\rstring"],
    ["test\"string", "test\\\"string"],
    ["test\'string", "test\\'string"],
    ["test \x00", "test \\0"]
  ],
  str,
  str_esc_theor,
  str_esc_real,
  i;
  
  for (i in strings_to_escape) {
    if (typeof i === 'string') {
      str = strings_to_escape[i][0];
      str_esc_theor = strings_to_escape[i][1];
      str_esc_real = conn.escape(strings_to_escape[i][0]);

      unittest.assertEqual(str_esc_theor, str_esc_real);
    }
  }
  
  conn.close();
});

unittest.test('conn.query("SHOW TABLES;")', function () {
  var conn = mysql_sync.createConnection(host, user, password, database),
    res;
  
  res = conn.query("SHOW TABLES;");
  unittest.assert(res);
  
  conn.close();
});

unittest.test('conn.query("SHOW TABLES;").fetchResult()', function () {
  var conn = mysql_sync.createConnection(host, user, password, database),
    res,
    tables;
  
  res = conn.query("SHOW TABLES;");
  tables = res.fetchResult();
  
  unittest.assert(tables);
  
  conn.close();
});

unittest.test('Drop table ' + test_table, function () {
  var conn = mysql_sync.createConnection(host, user, password, database),
    res;
  
  res = conn.query("DROP TABLE IF EXISTS " + test_table + ";");

  unittest.assertEqual(res, true);
  
  conn.close();
});

unittest.test('Create table ' + test_table, function () {
  var conn = mysql_sync.createConnection(host, user, password, database),
    res;
  
  res = conn.query("CREATE TABLE " + test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id));");

  unittest.assertEqual(res, true);
  
  conn.close();
});

unittest.test('Insert ' + insert_rows_count + ' rows into table ' + test_table, function () {
  var conn = mysql_sync.createConnection(host, user, password, database),
    res = true,
    random_number,
    random_boolean,
    i;
  
  for (i = 0; i < insert_rows_count; i += 1)
  {
    random_number = Math.round(Math.random() * 1000000);
    random_boolean = (Math.random() > 0.5) ? 1 : 0;
    res = conn.query("INSERT INTO " + test_table +
      " (random_number, random_boolean) VALUES ('" + random_number +
      "', '" + random_boolean + "');") && res;
  }

  unittest.assertEqual(res, true);
  
  conn.close();
});

unittest.test('conn.lastInsertId()', function () {
  var conn = mysql_sync.createConnection(host, user, password, database),
    res = true,
    random_number,
    random_boolean,
    last_insert_id,
    i;
  
  for (i = 0; i < insert_rows_count; i += 1)
  {
    random_number = Math.round(Math.random() * 1000000);
    random_boolean = (Math.random() > 0.5) ? 1 : 0;
    res = conn.query("INSERT INTO " + test_table +
      " (random_number, random_boolean) VALUES ('" + random_number +
      "', '" + random_boolean + "');") && res;
  }

  last_insert_id = conn.lastInsertId();
  
  unittest.assertEqual(last_insert_id, 2 * insert_rows_count);
  
  conn.close();
});

unittest.showResults();

/*
// TODO: Rewrite this tests

for( var i in show_tables_result )
{
  sys.puts(i + ": " + JSON.stringify(show_tables_result[i]));
}

sys.print("Test conn.lastInsertId(): ");
var insert_id = conn.lastInsertId();
if( insert_id == insert_rows_count )
{
  sys.print(" OK\n");
}
else
{
  sys.print(" FAILED [" + insert_id + " != " + insert_rows_count + "]\n");
}

sys.print("Run end get results of query 'SELECT * FROM " + test_table + " ORDER BY RAND() LIMIT 10': ");
conn.query("SELECT * FROM " + test_table + " ORDER BY RAND() LIMIT 10;");
var select_limit_result = conn.fetchResult();
sys.print("OK\n");
for( var i in select_limit_result )
{
  sys.puts(i + ": " + JSON.stringify(select_limit_result[i]));
}

sys.print("conn.warningCount() test: ");
sys.print(conn.warningCount());
sys.print(" OK\n");
*/

/*
// Deprecated code
sys.print("Reconnect to database " + reconnect_count + " times: ");
for( var i = 1; i <= reconnect_count; i++ )
{
  conn.close();
  conn.connect(host, user, password, database);
}
sys.print("OK\n");
*/

