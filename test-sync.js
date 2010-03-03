#!/usr/bin/env node
/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

var host = "localhost";
var user = "test";
var password = "";
var database = "test";
var database_denied = "mysql";
var test_table = "test_table";


var reconnect_count = 1000;
var insert_rows_count = 1000;

var sys = require("sys");
var mysql_sync = require("./mysql-sync");

var c;
var flag;

sys.print("Running tests for NodeJS syncronous MySQL binding...\n");

sys.print("Connect to database using mysql_sync.createConnection(): ");
c = mysql_sync.createConnection(host, user, password, database);
sys.print("OK\n");

sys.print("Close connection: ");
c.close();
sys.print("OK\n");

sys.print("Connect to denied database using mysql_sync.connect(): ");
flag = false;
try{
    c.connect(host, user, password, database_denied);
}
catch(exception)
{
    flag = true;
}
if(flag)
{
    sys.print("OK\n");
}
else
{
    sys.print("FAILED\n");
}

c.close();

sys.print("Connect to allowed database using mysql_sync.connect(): ");
c.connect(host, user, password, database);
sys.print("OK\n");

sys.print("Reconnect to database " + reconnect_count + " times: ");
for( i = 1; i <= reconnect_count; i++ )
{
    c.close();
    c.connect(host, user, password, database);
}
sys.print("OK\n");

//sys.print("Show tables query: ");
//c.query("SHOW TABLES;");
//sys.print("OK\n");

sys.print("Drop table " + test_table + ": ");
c.query("DROP TABLE IF EXISTS " + test_table + ";");
sys.print("OK\n");

sys.print("Create table " + test_table + ": ");
c.query("CREATE TABLE " + test_table + " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT, row_id INT(8) NOT NULL, random_number INT(8) NOT NULL, PRIMARY KEY (autoincrement_id));");
sys.print("OK\n");

sys.print("Insert " + reconnect_count + " rows into table " + test_table + ": ");
for( var i = 0; i < insert_rows_count; i++ )
{
    var random_number = Math.round(Math.random()*1000000);
    c.query("INSERT INTO " + test_table + " (row_id, random_number) VALUES ('" + i + "', '" + random_number + "');");
}
sys.print("OK\n");




sys.print("Close connection: ");
c.close();
sys.print("OK\n");

