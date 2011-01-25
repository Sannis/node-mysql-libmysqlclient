#!/usr/bin/env node
/*!
 * Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
 * See contributors list in README
 *
 * See license text in LICENSE file
 */

/**
 * Require modules and define variables
 */
var
  sys = require('sys'),
  mysql = require('../mysql-libmysqlclient'),
  conn,
  result,
  row,
  rows;

/**
 * Set database connection settings
 */
var
  host = "localhost",
  user = "test",
  password = "",
  database = "test",
  test_table = "test_table";

/**
 * Create connection
 */
conn = mysql.createConnectionSync();
conn.connectSync(host, user, password, database);

/**
 * Check connection status
 */
if (!conn.connectedSync()) {
  sys.puts("Connection error " + conn.connectErrno + ": " + conn.connectError);
  process.exit(1);
}

/**
 * Show tables in test database
 * Use low-level functions
 */
conn.realQuerySync("SHOW TABLES;");
result = conn.storeResultSync();

sys.puts("Tables in database '" + database + "':");
while ((row = result.fetchArraySync())) {
  sys.puts(row[0]);
}
sys.puts("");

result.freeSync();

/**
 * Show tables in test database
 * Use high-level functions
 */
result = conn.querySync("SHOW TABLES;");
rows = result.fetchAllSync();

sys.puts("Tables in database '" + database + "':");
sys.puts(sys.inspect(rows) + "\n");

/**
 * Show information about connection
 */
sys.puts("Information:");
sys.puts(sys.inspect(conn.getInfoSync()) + "\n");

/**
 * Run asynchronous queries and get results
 */
conn.query("SELECT * FROM " + test_table + ";", function (err, res) {
  if (err) {
    throw err;
  }
  
  res.fetchAll(function (err, rows) {
    if (err) {
      throw err;
    }
    
    sys.puts("Rows in table '" + database + "." + test_table + "':");
    sys.puts(sys.inspect(rows));
    
    // This isn't necessary since v1.2.0
    // See https://github.com/Sannis/node-mysql-libmysqlclient/issues#issue/60
    //res.freeSync();
  });
});

/**
 * Close connection on exit
 */
process.on('exit', function () {
  conn.closeSync();
});

