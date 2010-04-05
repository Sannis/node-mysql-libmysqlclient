Examples
========

Initialize
----------

First, import the library and open a database:

    var mysql_libmysqlclient = require("./node-mysql-libmysqlclient/mysql-libmysqlclient");
    var conn = mysql_libmysqlclient.createConnection(host, user, password, database);

Close and connect again
-----------------------

You can use all this argument in mysql_libmysqlclient.createConnection too.

    conn.close();
    conn.connect(servername, user, password, database, port, socket);

Execute query and fetch result
------------------------------

    var res = conn.query(query_string);
    var query_result_rows = res.fetchResult();

fetchResult() method returns array of objects. For example, for table with structure
    
    CREATE TABLE test_table (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT, random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL, PRIMARY KEY (autoincrement_id));

and query
    
    SELECT * FROM test_table ORDER BY RAND() LIMIT 3;
    
you will fetch

    {"autoincrement_id":"537","random_number":"222040","random_boolean":"0"},
    {"autoincrement_id":"47","random_number":"510740","random_boolean":"0"},
    {"autoincrement_id":"701","random_number":"158918","random_boolean":"0"}]

Utility functions
-----------------

    var info = conn.getInfo();
    for( var i in info ) sys.puts("info['" + i + "'] = " + info[i]);

This function get information about client and server versions and some other info:

    info['client_version'] = 50067
    info['client_info'] = 5.0.67
    info['server_version'] = 50067
    info['server_info'] = 5.0.67
    info['host_info'] = Localhost via UNIX socket
    info['proto_info'] = 10

escape() method escapes argument with mysql_real_escape_string() to make string safe for using in queries.

    var str = "test\nstring";
    var str_esc = conn.escape(str); // => "test\\nstring"

