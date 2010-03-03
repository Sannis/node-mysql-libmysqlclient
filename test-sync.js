#!/usr/local/bin/node

var sys = require("sys");
var mysql_sync = require("./mysql-sync");

var c = mysql_sync.createConnection("localhost", "test", "", "test");
c.close();

for( i = 0; i < 1000; i++ )
{
    c.connect("localhost", "test", "", "test");
    c.close();
}


