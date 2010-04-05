TODO: NodeJS MySQL/libmysqlclient bindings API
==============================================

Options
-------

    bool options (int $option , mixed $value);
    void set_local_infile_default (void);
    bool set_local_infile_handler (callback $read_func);

I think that this functions not so important at this moment.
We can implement MysqlSyncConn::setOption in first stable release,
and MysqlSyncConn::setLocalInfile{Default,Handler} later.


MysqlResult class
-----------------

currentField
dataSeek
free

Maybe: fetchRow<->fetchArray, fetchAssoc->fetchObject

