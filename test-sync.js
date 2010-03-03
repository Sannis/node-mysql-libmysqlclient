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
for( var i = 1; i <= reconnect_count; i++ )
{
    c.close();
    c.connect(host, user, password, database);
}
sys.print("OK\n");

sys.print("Get connection information: ");
var info = c.getInfo();
sys.print("OK\n");
for( var i in info )
{
    sys.puts("info['" + i + "'] = " + info[i]);
}

var strings_to_escape = [
    ["test string", "test string"],
    ["test\\string", "test\\\\string"],
    ["test\nstring", "test\\nstring"],
    ["test\rstring", "test\\rstring"],
    ["test\"string", "test\\\"string"],
    ["test\'string", "test\\'string"],
    ["test \x00", "test \\0"],
];
for( var i in strings_to_escape )
{
    var str = strings_to_escape[i][0];
    var str_esc_theor = strings_to_escape[i][1];
    var str_esc_real = c.escape(strings_to_escape[i][0]);
    
    sys.print("Test c.escape(): " + str + " -> " + str_esc_real);
    
    if( str_esc_real == str_esc_theor )
    {
        sys.print(" OK\n");
    }
    else
    {
        sys.print(" FAILED [Right string: " +  str_esc_theor + "]\n");
    }
}

sys.print("Run 'SHOW TABLES' query: ");
c.query("SHOW TABLES;");
sys.print("OK\n");

sys.print("Get 'SHOW TABLES' query result: ");
var show_tables_result = c.fetchResult();
sys.print("OK\n");
for( var i in show_tables_result )
{
    sys.puts(i + ": " + JSON.stringify(show_tables_result[i]));
}

sys.print("Drop table " + test_table + ": ");
c.query("DROP TABLE IF EXISTS " + test_table + ";");
sys.print("OK\n");

sys.print("Create table " + test_table + ": ");
c.query("CREATE TABLE " + test_table + " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT, random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL, PRIMARY KEY (autoincrement_id));");
sys.print("OK\n");

sys.print("Insert " + reconnect_count + " rows into table " + test_table + ": ");
for( var i = 0; i < insert_rows_count; i++ )
{
    var random_number = Math.round(Math.random()*1000000);
    var random_boolean = (Math.random() > 0.5) ? 1 : 0;
    c.query("INSERT INTO " + test_table + " (random_number, random_boolean) VALUES ('" + random_number + "', '" + random_boolean + "');");
}
sys.print("OK\n");

sys.print("Run end get results of query 'SELECT * FROM " + test_table + " ORDER BY RAND() LIMIT 10': ");
c.query("SELECT * FROM " + test_table + " ORDER BY RAND() LIMIT 10;");
var select_limit_result = c.fetchResult();
sys.print("OK\n");
for( var i in select_limit_result )
{
    sys.puts(i + ": " + JSON.stringify(select_limit_result[i]));
}

sys.print("Close connection: ");
c.close();
sys.print("OK\n");

