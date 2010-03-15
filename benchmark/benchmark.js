#!/usr/bin/env node
/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

var host = "localhost";
var user = "test";
var password = "";
var database = "test";
var test_table = "test_table";


var reconnects_count = 100000;
var escapes_count = 10000000;
var inserts_count = 200000;
var selects_count = 200;
var rows_to_select = 10;

var sys = require("sys");
var mysql_sync = require("../mysql-sync");


var conn;


// Utility functions

function round2(number)
{
    return Math.round(number*100)/100;
}

function do_benchmark(title, prepare_function, execute_function)
{
    var start, finish;
    
    if( typeof prepare_function == "function" )
    {
        var data = prepare_function();
        
        start = new Date();
        execute_function(data);
        finish = new Date();
    }
    else
    {
        start = new Date();
        execute_function();
        finish = new Date();
    }
    
    sys.puts(title + ": " + round2((finish.valueOf() - start.valueOf())/1000) + "s");
}


// Initialize connection for benchmarks

sys.puts("Init...");

conn = mysql_sync.createConnection(host, user, password, database);

var start = new Date();


// Benchmark 1: connect and disconnect

do_benchmark("Connect and disconnect(" + reconnects_count + ")", 0, function(){

    for( var i = 0; i < reconnects_count; i++ )
    {
        conn.close();
        conn.connect(host, user, password, database);
    }

});


// Benchmark 2: escape string

do_benchmark("Escape string(" + escapes_count + ")", 0, function(){

    // A bit utility code
    var string_to_escape = "test\\string\nwith\rmany\"symbols'and\x00nulls";
    var escaped_string;

    for( var i = 0; i < escapes_count; i++ )
    {
        escaped_string = conn.escape(string_to_escape);
    }

});


// Benchmark 3: inserts

do_benchmark("Inserts(" + inserts_count + ")", function(){

    conn.query("DROP TABLE IF EXISTS " + test_table + ";");
    conn.query("CREATE TABLE " + test_table + " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT, random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL, PRIMARY KEY (autoincrement_id));");

    var rows = [];

    for( var i = 0; i < inserts_count; i++ )
    {
        rows[i] = { 'random_number': Math.round(Math.random()*1000000), 'random_boolean': ( (Math.random() > 0.5) ? 1 : 0 ) };
    }
    
    return rows;

}, function(rows){

    for( var i = 0; i < inserts_count; i++ )
    {
        conn.query("INSERT INTO " + test_table + " (random_number, random_boolean) VALUES ('" + rows[i].random_number + "', '" + rows[i].random_boolean + "');");
    }

});


// Benchmark 3: selects

do_benchmark("Selects(" + selects_count + ", " + rows_to_select + ")", function(){

    var limits = [];

    for( var i = 0; i < selects_count; i++ )
    {
        limits[i] = Math.round(Math.random()*(inserts_count-rows_to_select));
    }
    
    return limits;

}, function(limits){

    var rows = new Array(selects_count);
    
    for( var i = 0; i < selects_count; i++ )
    {
        conn.query("SELECT * FROM " + test_table + " ORDER BY RAND() LIMIT " + limits[i] + ", " + rows_to_select + ";");
        rows[i] = conn.fetchResult();
    }

});


// Finish benchmarks

var finish = new Date();

sys.puts("Finish.");

conn.close();

sys.puts("Full time: " + round2((finish.valueOf() - start.valueOf())/1000) + "s");

