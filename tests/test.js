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

unittest.showResults();

/*
// TODO: Rewrite this tests
sys.print("Run 'SHOW TABLES' query: ");
conn.query("SHOW TABLES;");
sys.print("OK\n");

sys.print("Get 'SHOW TABLES' query result: ");
var show_tables_result = conn.fetchResult();
sys.print("OK\n");
for( var i in show_tables_result )
{
  sys.puts(i + ": " + JSON.stringify(show_tables_result[i]));
}

sys.print("Drop table " + test_table + ": ");
conn.query("DROP TABLE IF EXISTS " + test_table + ";");
sys.print("OK\n");

sys.print("Create table " + test_table + ": ");
conn.query("CREATE TABLE " + test_table + " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT, random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL, PRIMARY KEY (autoincrement_id));");
sys.print("OK\n");

sys.print("Insert " + insert_rows_count + " rows into table " + test_table + ": ");
for( var i = 0; i < insert_rows_count; i++ )
{
  var random_number = Math.round(Math.random()*1000000);
  var random_boolean = (Math.random() > 0.5) ? 1 : 0;
  conn.query("INSERT INTO " + test_table + " (random_number, random_boolean) VALUES ('" + random_number + "', '" + random_boolean + "');");
}
sys.print("OK\n");

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

