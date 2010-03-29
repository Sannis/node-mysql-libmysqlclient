NodeJS mysql-sync bindings API
==============================

Introduction
------------

This file contains overview of node-mysql-sync API.


MysqlSyncConn class
-------------------

    class MysqlSyncConn {
        /* Constructor */
        require('mysql-sync').createConnection(servername, user, password, dbname, port, socket);
        
        /* Methods */
        Integer affectedRows();
        Boolean changeUser(user, password, dbname);
        Boolean connect(hostname, user, password, dbname, port, socket);
        Integer connectErrno();
        String connectError();
        Boolean close();
        Integer errno();
        String error();
        String escape();
        Object getInfo();
        Integer lastInsertId();
        Boolean ping();
        MysqlSyncRes query();
        Integer warningCount();
    }


Connection, options, errors and information
-------------------------------------------

### Connection ###

    require('mysql-sync').createConnection(servername, user, password, dbname, port, socket);
    
    Boolean changeUser(user, password, dbname);
    Boolean connect(hostname, user, password, dbname, port, socket);
    Integer connectErrno();
    String connectError();
    Boolean close();
    Object getCharset();
    String getCharsetName();
    Boolean ping();
    Boolean selectDb (dbname);
    Boolean setCharset (charset);
    Undefined setSsl(key, cert, ca, capath, cipher);
    

### Options ###

Nothing yet.


### Errors ###

    Integer errno();
    String error();
    String sqlState;


### Information ###

    Object getInfo();
    String getInfoString();
    Array getWarnings();
    Integer warningCount();

	Undefined debug(message);
	Boolean dumpDebugInfo();
	String stat();


Query
-----

    Integer affectedRows();
    String escape(escapestr);
    Integer fieldCount();
    Integer lastInsertId();
    Boolean multiMoreResults();
    Boolean multiNextResult()
    Boolean multiRealQuery(query);
    MysqlSyncRes query(query);
    Boolean realQuery(query);
    MysqlSyncRes storeResult();
    MysqlSyncRes useResult();


Statements
----------

    MysqlSyncStmt initStmt (void);


Transactions
------------

Nothing yet.


Other
-----

Nothing yet.


MysqlSyncStmt class
-------------------

    class MysqlSyncStmt {
        /* Methods */
        Boolean prepare(query);
    }



