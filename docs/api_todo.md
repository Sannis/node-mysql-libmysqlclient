TODO: NodeJS MySQL/libmysqlclient bindings API
==============================================

Connection, options, errors and information
-------------------------------------------

### Options ###

    bool options (int $option , mixed $value);
    void set_local_infile_default (void);
    bool set_local_infile_handler (callback $read_func);
