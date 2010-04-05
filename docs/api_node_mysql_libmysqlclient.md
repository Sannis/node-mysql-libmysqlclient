NodeJS MySQL/libmysqlclient bindings API
========================================

Introduction
------------

This file contains overview of node-mysql-sync API.


MysqlConn class
---------------

    class MysqlConn {
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
        MysqlResult query();
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
    MysqlResult query(query);
    Boolean realQuery(query);
    MysqlResult storeResult();
    MysqlResult useResult();


Statements
----------

    MysqlStatement initStmt (void);


Transactions
------------

    Boolean autoCommit(mode);
    Boolean commit();
    Boolean rollback();


Other
-----

    Integer threadId();
    Boolean threadKill(pid);
    Boolean threadSafe();


MysqlResult class
-----------------

    class MysqlResult {
        /* Methods */
        Array fetchAll();
        Array fetchArray();
        Object fetchField();
        Object fetchFieldDirect();
        Array fetchFields();
        Array fetchLengths();
        Object fetchObject();
        Integer fieldCount();
        Undefined fieldSeek();
        Integer fieldTell();
        Integer numRows();
    }


MysqlStatement class
--------------------

    class MysqlStatement {
        /* Methods */
        Boolean prepare(query);
    }



