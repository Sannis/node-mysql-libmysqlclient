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


var reconnect_count = 10000;
var insert_rows_count = 10000;

var sys = require("sys");
var mysql_sync = require("../mysql-sync");

var conn;
var flag;

sys.print("Running tests for NodeJS syncronous MySQL binding...\n");

sys.print("Connect to database using mysql_synconn.createConnection(): ");
conn = mysql_sync.createConnection(host, user, password, database);
sys.print("OK\n");

sys.print("Close connection: ");
conn.close();
sys.print("OK\n");

sys.print("Connect to denied database using mysql_sync.connect(): ");
if(conn.connect(host, user, password, database_denied))
{
    sys.print("FAILED\n");
}
else
{
    sys.print("OK\n");
    sys.puts("Error: [" + conn.errno() + "] " + conn.error());
}

conn.close();

sys.print("Connect to allowed database using mysql_synconn.connect(): ");
conn.connect(host, user, password, database);
sys.print("OK\n");

sys.print("Reconnect to database " + reconnect_count + " times: ");
for( var i = 1; i <= reconnect_count; i++ )
{
    conn.close();
    conn.connect(host, user, password, database);
}
sys.print("OK\n");

sys.print("Get connection information: ");
var info = conn.getInfo();
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
    var str_esc_real = conn.escape(strings_to_escape[i][0]);
    
    sys.print("Test conn.escape(): " + str + " -> " + str_esc_real);
    
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
sys.print(" OK\n"); //TODO: new test for conn.warningCount()

sys.print("Close connection: ");
conn.close();
sys.print("OK\n");

