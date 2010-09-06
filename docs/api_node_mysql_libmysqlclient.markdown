NodeJS MySQL/libmysqlclient bindings API
========================================

Introduction
------------

This file contains overview of node-mysql-sync API.


MysqlConn class
---------------

    class MysqlConn {
        /* Constructor */
        require('mysql-sync').createConnectionSync(servername, user, password, dbname, port, socket);
        
        /* Properties */
        Integer connectErrno;
        String connectError;
        
        /* Methods */
        Integer affectedRowsSync();
        Boolean changeUserSync(user, password, dbname);
        Boolean connect(hostname, user, password, dbname, port, socket);
        Boolean connectSync(hostname, user, password, dbname, port, socket);
        Boolean closeSync();
        Integer errnoSync();
        String errorSync();
        String escapeSync(string);
        Object getInfoSync();
        Integer lastInsertIdSync();
        Boolean pingSync();
        MysqlResult query(query);
        MysqlResult querySync(query);
        Integer warningCountSync();
    }


Connection, options, errors and information
-------------------------------------------

### Connection ###

    require('mysql-sync').createConnectionSync(servername, user, password, dbname, port, socket);
    
    Integer connectErrno;
    String connectError;
    
    Boolean changeUserSync(user, password, dbname);
    Boolean connect(hostname, user, password, dbname, port, socket);
    Boolean connectSync(hostname, user, password, dbname, port, socket);
    Boolean closeSync();
    Object getCharsetSync();
    String getCharsetNameSync();
    Boolean pingSync();
    Boolean selectDbSync(dbname);
    Boolean setCharsetSync(charset);
    Undefined setSslSync(key, cert, ca, capath, cipher);
    

### Options ###

Nothing yet.


### Errors ###

    Integer errnoSync();
    String errorSync();
    String sqlStateSync();


### Information ###

    Object getInfoSync();
    String getInfoStringSync();
    Array getWarningsSync();
    Integer warningCountSync();

	Undefined debugSync(message);
	Boolean dumpDebugInfoSync();
	String statSync();


Query
-----

    Integer affectedRowsSync();
    String escapeSync(escapestr);
    Integer fieldCountSync();
    Integer lastInsertIdSync();
    Boolean multiMoreResultsSync();
    Boolean multiNextResultSync()
    Boolean multiRealQuerySync(query);
    MysqlResult query(query);
    MysqlResult querySync(query);
    Boolean realQuerySync(query);
    MysqlResult storeResultSync();
    MysqlResult useResultSync();


Statements
----------

    MysqlStatement initStmt (void);


Transactions
------------

    Boolean autoCommitSync(mode);
    Boolean commitSync();
    Boolean rollbackSync();


Other
-----

    Integer threadIdSync();
    Boolean threadKillSync(pid);
    Boolean threadSafeSync();


MysqlResult class
-----------------

    class MysqlResult {
        /* Properties */
        Integer fieldCount;
        
        /* Methods */
        Undefined dataSeekSync();
        Array fetchAll();
        Array fetchAllSync();
        Array fetchArraySync();
        Object fetchFieldSync();
        Object fetchFieldDirectSync();
        Array fetchFieldsSync();
        Array fetchLengthsSync();
        Object fetchObjectSync();
        
        Undefined fieldSeekSync();
        Integer fieldTellSync();
        Undefined freeSync();
        Integer numRowsSync();
    }


MysqlStatement class
--------------------

    class MysqlStatement {
        /* Methods */
        Boolean prepareSync(query);
    }

