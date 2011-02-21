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
  http = require('http'),
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

if (!conn.connectedSync()) {
  sys.puts("Connection error " + conn.connectErrno + ": " + conn.connectError);
  process.exit(1);
}

process.on('exit', function () {
  conn.closeSync();
});

/**
 * Create table
 */
conn.querySync("DROP TABLE IF EXISTS " + test_table + ";");
conn.querySync("CREATE TABLE " + test_table +
  " (id INT(8) NOT NULL AUTO_INCREMENT, " +
  "time BIGINT(13) NOT NULL, " +
  "PRIMARY KEY(id));");

/**
 * Create HTTP server
 */
http.createServer(function (request, response) {
  var t = Date.now();
  
  conn.query("INSERT INTO " + test_table + "(time) VALUES ('" + t + "');", function (err, res) {
    if (err) {
      response.writeHead(500, {'Content-Type': 'text/plain'});
      response.end('500: Internal server error\nMySQL error: ' + err.message);
      throw err;
    }
    
    response.writeHead(200, {'Content-Type': 'text/plain'});
    response.end('Hello World: ' + t);
  });
}).listen(8124);

console.log('Server running at http://127.0.0.1:8124/');

setInterval(function () {
  console.log(Date.now());
  console.log(sys.inspect(process.memoryUsage()));
}, 30 * 1000);

