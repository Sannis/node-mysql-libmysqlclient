PHP MySQLi API
==============

Introduction
------------

This file contains overview of PHP 5 MySQLi API on 12th of March 2010.
I expect to implement similar functions in Node.js bindings.


MySQLi class
------------

    class mysqli {
        /* Properties */
        int $affected_rows;
        string $client_info;
        int $client_version;
        string $connect_errno;
        string $connect_error;
        int $errno;
        string $error;
        int $field_count;
        int $client_version;
        string $host_info;
        string $protocol_version;
        string $server_info;
        int $server_version;
        string $info;
        mixed $insert_id;
        string $sqlstate;
        int $thread_id;
        int $warning_count;
        
        /* Constructor */
        __construct (
            [ string $host [,
            string $username [,
            string $passwd [,
            string $dbname [,
            int $port [,
            string $socket ]]]]]]
       );
        
        /* Methods */
        bool autocommit (bool $mode);
        bool change_user (string $user , string $password , string $database);
        string character_set_name (void);
        bool close (void);
        bool commit (void);
        bool debug (string $message);
        bool dump_debug_info (void);
        object get_charset (void);
        bool get_connection_stats (void);
        mysqli_warnings get_warnings (void);
        mysqli init (void);
        bool kill (int $processid);
        bool more_results (void);
        bool multi_query (string $query);
        bool next_result (void);
        bool options (int $option , mixed $value);
        bool ping (void);
        int poll (array &$read , array &$error , array &$reject , int $sec [, int $usec ]);
        mysqli_stmt prepare (string $query);
        mixed query (string $query [, int $resultmode ]);
        bool real_connect ([ string $host [, string $username [, string $passwd [, string $dbname [, int $port [, string $socket [, int $flags ]]]]]]]);
        string escape_string (string $escapestr);
        bool real_query (string $query);
        mysqli_result reap_async_query (void);
        bool rollback (void);
        bool select_db (string $dbname);
        bool set_charset (string $charset);
        void set_local_infile_default (mysqli $link);
        bool set_local_infile_handler (mysqli $link , callback $read_func);
        bool ssl_set (string $key , string $cert , string $ca , string $capath , string $cipher);
        string stat (void);
        mysqli_stmt stmt_init (void);
        mysqli_result store_result (void);
        bool thread_safe (void);
        mysqli_result use_result (void);
    }


Connection, options, errors and information
-------------------------------------------

### Connection ###

    __construct (
        [ string $host [,
        string $username [,
        string $passwd [,
        string $dbname [,
        int $port [,
        string $socket ]]]]]]
    );
    
    string $connect_errno;
    string $connect_error;
    
    bool change_user (string $user , string $password , string $database);
    bool close (void);
    mysqli init (void);
    bool ping (void);
    bool real_connect ([ string $host [, string $username [, string $passwd [, string $dbname [, int $port [, string $socket [, int $flags ]]]]]]]);
    bool select_db (string $dbname);
    bool ssl_set (string $key , string $cert , string $ca , string $capath , string $cipher);
    
    string character_set_name (void);
    object get_charset (void);
    bool set_charset (string $charset);


### Options ###

    bool options (int $option , mixed $value);
    void set_local_infile_default (void);
    bool set_local_infile_handler (callback $read_func);


### Errors ###

    int $errno;
    string $error;
    string $sqlstate;


### Information ###

    string $client_info;
    int $client_version;
    string $host_info;
    string $protocol_version;
    string $server_info;
    int $server_version;
    
    string $info;
    int $warning_count;
    mysqli_warnings get_warnings (void);
    
    bool debug (string $message);
    bool dump_debug_info (void);
    bool get_connection_stats (void);
    string stat (void);


Query
-----

    int $affected_rows;
    int $field_count;
    mixed $insert_id;
    
    bool more_results (void);
    bool multi_query (string $query);
    bool next_result (void);
    mysqli_stmt prepare (string $query);
    mixed query (string $query [, int $resultmode ]);
    string escape_string (string $escapestr);
    string real_escape_string (string $escapestr);
    bool real_query (string $query);
    mysqli_stmt stmt_init (void);
    mysqli_result store_result (void);
    mysqli_result use_result (void);


Transactions
------------

    bool autocommit (bool $mode);
    bool commit (void);
    bool rollback (void);


Other
-----

    int $thread_id;
    bool kill (int $processid);
    bool thread_safe (void);    


    int poll (array &$read , array &$error , array &$reject , int $sec [, int $usec ]);
    mysqli_result reap_async_query (void);

