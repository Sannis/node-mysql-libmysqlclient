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
  util = require('util'),
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
  util.puts("Connection error " + conn.connectErrno + ": " + conn.connectError);
  process.exit(1);
}

/**
 * Show tables in test database
 * Use low-level functions
 */
conn.realQuerySync("SHOW TABLES;");
result = conn.storeResultSync();

util.puts("Tables in database '" + database + "':");
while ((row = result.fetchRowSync({"asArray": true}))) {
  util.puts(row[0]);
}
util.puts("");

result.freeSync();

/**
 * Show tables in test database
 * Use high-level functions
 */
result = conn.querySync("SHOW TABLES;");
rows = result.fetchAllSync();

util.puts("Tables in database '" + database + "':");
util.puts(util.inspect(rows) + "\n");

/**
 * Show information about connection
 */
util.puts("Information:");
util.puts(util.inspect(conn.getInfoSync()) + "\n");

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
    
    util.puts("Rows in table '" + database + "." + test_table + "':");
    util.puts(util.inspect(rows));
    
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

