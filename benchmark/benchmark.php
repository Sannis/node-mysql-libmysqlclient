#!/usr/bin/env php
<?php
/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

$host = "localhost";
$user = "test";
$password = "";
$database = "test";
$test_table = "test_table";


$reconnects_count = 100000;
$escapes_count = 10000000;
$inserts_count = 200000;
$selects_count = 100;
$rows_to_select = 10;


$conn;


// Utility functions

function sys_puts($str)
{
    print $str."\n";
}

function my_random()
{
    return rand()/getrandmax();
}

function do_benchmark($title, $prepare_function, $execute_function)
{
    $start;
    $finish;
    
    if( $prepare_function )
    {
        $data = call_user_func($prepare_function);
        
        $start = microtime(true);
        call_user_func($execute_function, $data);
        $finish = microtime(true);
    }
    else
    {
        $start = microtime(true);
        call_user_func($execute_function);
        $finish = microtime(true);
    }
    
    sys_puts($title.": ".round(($finish - $start), 2)."s");
}


// Benchmarking functions

function do_benchmark_connect_and_disconnect()
{
    global $conn, $reconnects_count, $host, $user, $password, $database;
    
    for( $i = 0; $i < $reconnects_count; $i++ )
    {
        mysql_close($conn);
        $conn = mysql_connect($host, $user, $password);
        mysql_select_db($database , $conn);
    }

}

function do_benchmark_escape_string()
{
    global $conn, $escapes_count;
    
    // A bit utility code
    $string_to_escape = "test\\string\nwith\rmany\"symbols'and\x00nulls";
    $escaped_string;

    for( $i = 1; $i <= $escapes_count; $i++ )
    {
        $escaped_string = mysql_real_escape_string($string_to_escape, $conn);
    }
}

function do_benchmark_insert_prepare()
{
    global $conn, $test_table, $inserts_count;
    
    mysql_query("DROP TABLE IF EXISTS ".$test_table.";", $conn);
    mysql_query("CREATE TABLE ".$test_table." (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT, random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL, PRIMARY KEY (autoincrement_id));", $conn);

    $rows = array();

    for( $i = 0; $i < $inserts_count; $i++ )
    {
        $rows[$i] = array( 'random_number' => intval(my_random()*1000000), 'random_boolean' => ( (my_random() > 0.5) ? 1 : 0 ) );
    }
    
    return $rows;
}

function do_benchmark_insert($rows)
{
    global $conn, $test_table, $inserts_count;
    
    for( $i = 0; $i < $inserts_count; $i++ )
    {
        mysql_query("INSERT INTO ".$test_table." (random_number, random_boolean) VALUES ('".$rows[$i]['random_number']."', '".$rows[$i]['random_boolean']."');", $conn);
    }
}

function do_benchmark_select_prepare()
{
    global $conn, $selects_count, $inserts_count, $rows_to_select;
    
    $limits = array();

    for( $i = 0; $i < $selects_count; $i++ )
    {
        $limits[$i] = intval(my_random()*($inserts_count-$rows_to_select));
    }
    
    return $limits;
}

function do_benchmark_select($limits)
{
    global $conn, $test_table, $selects_count, $rows_to_select;
    
    $rows = array();
    
    for( $i = 0; $i < $selects_count; $i++ )
    {
        $r = mysql_query("SELECT * FROM ".$test_table." ORDER BY RAND() LIMIT ".$limits[$i].", ".$rows_to_select.";", $conn);
        
        $rows[$i] = array();
        
        while($row = mysql_fetch_array($r))
        {
            $rows[$i][] = $row;
        }
    }
}

// Initialize connection for benchmarks

sys_puts("Init...");

$conn = mysql_connect($host, $user, $password);
mysql_select_db($database, $conn);

$start = microtime(true);


// Benchmark 1: connect and disconnect

do_benchmark("Connect and disconnect(".$reconnects_count.")", 0, "do_benchmark_connect_and_disconnect");


// Benchmark 2: escape string

do_benchmark("Escape string(".$escapes_count.")", 0, "do_benchmark_escape_string");


// Benchmark 3: inserts

do_benchmark("Inserts(".$inserts_count.")", "do_benchmark_insert_prepare", "do_benchmark_insert");


// Benchmark 3: selects

do_benchmark("Selects(".$selects_count.", ".$rows_to_select.")", "do_benchmark_select_prepare", "do_benchmark_select");


// Finish benchmarks

$finish = microtime(true);

sys_puts("Finish.");

mysql_close($conn);

sys_puts("Full time: ".round(($finish - $start), 2)."s");

