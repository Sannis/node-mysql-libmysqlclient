/*!
 * Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
 * See contributors list in README
 *
 * See license text in LICENSE file
 */

/**
 * Include headers
 *
 * @ignore
 */
#include "./mysql_bindings_connection.h"
#include "./mysql_bindings_result.h"
#include "./mysql_bindings_statement.h"

/**
 * Init V8 structures for MysqlConnection class
 *
 * @ignore
 */
Persistent<FunctionTemplate> MysqlConnection::constructor_template;

void MysqlConnection::Init(Handle<Object> target) {
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New(New);

    // Constructor
    constructor_template = Persistent<FunctionTemplate>::New(t);
    constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
    constructor_template->SetClassName(String::NewSymbol("MysqlConnection"));

    Local<ObjectTemplate> instance_template =
        constructor_template->InstanceTemplate();

    // Constants for connect flags
    NODE_DEFINE_CONSTANT(instance_template, CLIENT_COMPRESS);
    NODE_DEFINE_CONSTANT(instance_template, CLIENT_FOUND_ROWS);
    NODE_DEFINE_CONSTANT(instance_template, CLIENT_IGNORE_SIGPIPE);
    NODE_DEFINE_CONSTANT(instance_template, CLIENT_IGNORE_SPACE);
    NODE_DEFINE_CONSTANT(instance_template, CLIENT_INTERACTIVE);
    // Not yet implemented
    // NODE_DEFINE_CONSTANT(instance_template, CLIENT_LOCAL_FILES);
    NODE_DEFINE_CONSTANT(instance_template, CLIENT_MULTI_RESULTS);
    NODE_DEFINE_CONSTANT(instance_template, CLIENT_MULTI_STATEMENTS);
    NODE_DEFINE_CONSTANT(instance_template, CLIENT_NO_SCHEMA);
    // Unused by MySQL
    // NODE_DEFINE_CONSTANT(instance_template, CLIENT_ODBC);
    // This option should not be set by application programs;
    // it is set internally in the client library. Instead,
    // use setSslSync() before calling connect() or connectSync().
    // NODE_DEFINE_CONSTANT(instance_template, CLIENT_SSL);
    // Known issue: conn.CLIENT_REMEMBER_OPTIONS === -2147483648
    NODE_DEFINE_CONSTANT(instance_template, CLIENT_REMEMBER_OPTIONS);

    // Constants for setOption
    NODE_DEFINE_CONSTANT(instance_template, MYSQL_INIT_COMMAND);
    NODE_DEFINE_CONSTANT(instance_template, MYSQL_OPT_COMPRESS);
    NODE_DEFINE_CONSTANT(instance_template, MYSQL_OPT_CONNECT_TIMEOUT);
    // Unused, embedded
    // NODE_DEFINE_CONSTANT(MYSQL_OPT_GUESS_CONNECTION);
    // Not yet implemented
    // NODE_DEFINE_CONSTANT(instance_template, MYSQL_OPT_LOCAL_INFILE);
    // Unused, windows
    // NODE_DEFINE_CONSTANT(instance_template, MYSQL_OPT_NAMED_PIPE);
    NODE_DEFINE_CONSTANT(instance_template, MYSQL_OPT_PROTOCOL);
    NODE_DEFINE_CONSTANT(instance_template, MYSQL_OPT_READ_TIMEOUT);
    NODE_DEFINE_CONSTANT(instance_template, MYSQL_OPT_RECONNECT);
    // Unused, embedded
    // NODE_DEFINE_CONSTANT(MYSQL_SET_CLIENT_IP);
    NODE_DEFINE_CONSTANT(instance_template, MYSQL_OPT_SSL_VERIFY_SERVER_CERT);
    // Unused, embedded
    // NODE_DEFINE_CONSTANT(MYSQL_OPT_USE_EMBEDDED_CONNECTION);
    // Unused, embedded
    // NODE_DEFINE_CONSTANT(MYSQL_OPT_USE_REMOTE_CONNECTION);
    // Unused by MySQL
    // NODE_DEFINE_CONSTANT(MYSQL_OPT_USE_RESULT);
    NODE_DEFINE_CONSTANT(instance_template, MYSQL_OPT_WRITE_TIMEOUT);
    NODE_DEFINE_CONSTANT(instance_template, MYSQL_READ_DEFAULT_FILE);
    NODE_DEFINE_CONSTANT(instance_template, MYSQL_READ_DEFAULT_GROUP);
    NODE_DEFINE_CONSTANT(instance_template, MYSQL_REPORT_DATA_TRUNCATION);
    NODE_DEFINE_CONSTANT(instance_template, MYSQL_SECURE_AUTH);
    NODE_DEFINE_CONSTANT(instance_template, MYSQL_SET_CHARSET_DIR);
    NODE_DEFINE_CONSTANT(instance_template, MYSQL_SET_CHARSET_NAME);
    // Unused, windows
    // NODE_DEFINE_CONSTANT(instance_template, MYSQL_SHARED_MEMORY_BASE_NAME);

    // Properties
    instance_template->SetAccessor(V8STR("connectErrno"), ConnectErrnoGetter);
    instance_template->SetAccessor(V8STR("connectError"), ConnectErrorGetter);

    // Methods
    ADD_PROTOTYPE_METHOD(connection, affectedRowsSync, AffectedRowsSync);
    ADD_PROTOTYPE_METHOD(connection, autoCommitSync, AutoCommitSync);
    ADD_PROTOTYPE_METHOD(connection, changeUserSync, ChangeUserSync);
    ADD_PROTOTYPE_METHOD(connection, commitSync, CommitSync);
    ADD_PROTOTYPE_METHOD(connection, connect, Connect);
    ADD_PROTOTYPE_METHOD(connection, connectSync, ConnectSync);
    ADD_PROTOTYPE_METHOD(connection, connectedSync, ConnectedSync);
    ADD_PROTOTYPE_METHOD(connection, closeSync, CloseSync);
    ADD_PROTOTYPE_METHOD(connection, debugSync, DebugSync);
    ADD_PROTOTYPE_METHOD(connection, dumpDebugInfoSync, DumpDebugInfoSync);
    ADD_PROTOTYPE_METHOD(connection, errnoSync, ErrnoSync);
    ADD_PROTOTYPE_METHOD(connection, errorSync, ErrorSync);
    ADD_PROTOTYPE_METHOD(connection, escapeSync, EscapeSync);
    ADD_PROTOTYPE_METHOD(connection, fieldCountSync, FieldCountSync);
    ADD_PROTOTYPE_METHOD(connection, getCharsetSync, GetCharsetSync);
    ADD_PROTOTYPE_METHOD(connection, getCharsetNameSync, GetCharsetNameSync);
    ADD_PROTOTYPE_METHOD(connection, getClientInfoSync, GetClientInfoSync);
    ADD_PROTOTYPE_METHOD(connection, getInfoSync, GetInfoSync);
    ADD_PROTOTYPE_METHOD(connection, getInfoStringSync, GetInfoStringSync);
    ADD_PROTOTYPE_METHOD(connection, getWarningsSync, GetWarningsSync);
    ADD_PROTOTYPE_METHOD(connection, initSync, InitSync);
    ADD_PROTOTYPE_METHOD(connection, initStatementSync, InitStatementSync);
    ADD_PROTOTYPE_METHOD(connection, lastInsertIdSync, LastInsertIdSync);
    ADD_PROTOTYPE_METHOD(connection, multiMoreResultsSync, MultiMoreResultsSync);
    ADD_PROTOTYPE_METHOD(connection, multiNextResultSync, MultiNextResultSync);
    ADD_PROTOTYPE_METHOD(connection, multiRealQuerySync, MultiRealQuerySync);
    ADD_PROTOTYPE_METHOD(connection, pingSync, PingSync);
    ADD_PROTOTYPE_METHOD(connection, query, Query);
    ADD_PROTOTYPE_METHOD(connection, querySend, QuerySend);
    ADD_PROTOTYPE_METHOD(connection, querySync, QuerySync);
    ADD_PROTOTYPE_METHOD(connection, realConnectSync, RealConnectSync);
    ADD_PROTOTYPE_METHOD(connection, realQuerySync, RealQuerySync);
    ADD_PROTOTYPE_METHOD(connection, rollbackSync, RollbackSync);
    ADD_PROTOTYPE_METHOD(connection, selectDbSync, SelectDbSync);
    ADD_PROTOTYPE_METHOD(connection, setCharsetSync, SetCharsetSync);
    ADD_PROTOTYPE_METHOD(connection, setOptionSync, SetOptionSync);
    ADD_PROTOTYPE_METHOD(connection, setSslSync, SetSslSync);
    ADD_PROTOTYPE_METHOD(connection, sqlStateSync, SqlStateSync);
    ADD_PROTOTYPE_METHOD(connection, statSync, StatSync);
    ADD_PROTOTYPE_METHOD(connection, storeResultSync, StoreResultSync);
    ADD_PROTOTYPE_METHOD(connection, threadIdSync, ThreadIdSync);
    ADD_PROTOTYPE_METHOD(connection, threadSafeSync, ThreadSafeSync);
    ADD_PROTOTYPE_METHOD(connection, useResultSync, UseResultSync);
    ADD_PROTOTYPE_METHOD(connection, warningCountSync, WarningCountSync);

    // Make it visible in JavaScript
    target->Set(String::NewSymbol("MysqlConnection"),
                constructor_template->GetFunction());
}

bool MysqlConnection::Connect(const char* hostname,
                        const char* user,
                        const char* password,
                        const char* dbname,
                        uint32_t port,
                        const char* socket,
                        uint64_t flags) {
    if (this->_conn) {
        return false;
    }

    this->_conn = mysql_init(NULL);

    if (!this->_conn) {
        this->connected = false;
        return false;
    }

    bool unsuccessful = !mysql_real_connect(this->_conn,
                            hostname,
                            user,
                            password,
                            dbname,
                            port,
                            socket,
                            flags);

    if (unsuccessful) {
        this->connect_errno = mysql_errno(this->_conn);
        this->connect_error = mysql_error(this->_conn);

        mysql_close(this->_conn);
        this->connected = false;
        this->_conn = NULL;
        return false;
    }

    this->connected = true;
    return true;
}

bool MysqlConnection::RealConnect(const char* hostname,
                            const char* user,
                            const char* password,
                            const char* dbname,
                            uint32_t port,
                            const char* socket,
                            uint64_t flags) {
    if (!this->_conn) {
        return false;
    }

    if (this->connected) {
        return false;
    }

    bool unsuccessful = !mysql_real_connect(this->_conn,
                                            hostname,
                                            user,
                                            password,
                                            dbname,
                                            port,
                                            socket,
                                            flags);

    if (unsuccessful) {
        this->connect_errno = mysql_errno(this->_conn);
        this->connect_error = mysql_error(this->_conn);

        mysql_close(this->_conn);
        this->_conn = NULL;
        this->connected = false;
        return false;
    }

    // MYSQL_OPT_RECONNECT option is modified by mysql_real_connect
    // due to bug in MySQL < 5.1.6
    // Save it state and repeat mysql_options after connect
    // See issue #66
#if MYSQL_VERSION_ID >= 50013 && MYSQL_VERSION_ID < 50106
    unsuccessful = mysql_options(this->_conn, MYSQL_OPT_RECONNECT,
                            static_cast<const char*>(&this->opt_reconnect));
    if (unsuccessful) {
        this->connect_errno = mysql_errno(this->_conn);
        this->connect_error = mysql_error(this->_conn);

        mysql_close(this->_conn);
        this->_conn = NULL;
        this->connected = false;
        return false;
    }
#endif

    this->connected = true;
    return true;
}

void MysqlConnection::Close() {
    if (this->_conn) {
        mysql_close(this->_conn);
        this->_conn = NULL;
        this->connected = false;
        // this->multi_query = false;
        this->opt_reconnect = false;
        this->connect_errno = 0;
        this->connect_error = NULL;
    }
}

MysqlConnection::MysqlConnection(): ObjectWrap() {
    this->_conn = NULL;
    this->connected = false;
    this->multi_query = false;
    this->opt_reconnect = false;
    this->connect_errno = 0;
    this->connect_error = NULL;
    pthread_mutex_init(&this->query_lock, NULL);
}

MysqlConnection::~MysqlConnection() {
    this->Close();
    pthread_mutex_destroy(&this->query_lock);
}

/**
 * Creates new MysqlConnection object
 *
 * @constructor
 */
Handle<Value> MysqlConnection::New(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = new MysqlConnection();
    conn->Wrap(args.This());

    return args.This();
}

/**
 * Gets last connect error number
 *
 * @getter
 * @return {Integer}
 */
Handle<Value> MysqlConnection::ConnectErrnoGetter(Local<String> property,
                                            const AccessorInfo &info) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    return scope.Close(Integer::NewFromUnsigned(conn->connect_errno));
}

/**
 * Gets last connect error string
 *
 * @getter
 * @return {Integer}
 */
Handle<Value> MysqlConnection::ConnectErrorGetter(Local<String> property,
                                            const AccessorInfo &info) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(info.Holder());

    return scope.Close(V8STR(conn->connect_error ? conn->connect_error : ""));
}

/**
 * Gets number of affected rows in previous operation
 *
 * @return {Integer}
 */
Handle<Value> MysqlConnection::AffectedRowsSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_CONNECTED;

    my_ulonglong affected_rows = mysql_affected_rows(conn->_conn);

    if (affected_rows == ((my_ulonglong)-1)) {
        return scope.Close(Integer::New(-1));
    }

    return scope.Close(Integer::New(affected_rows));
}

/**
 * Sets autocommit mode
 *
 * @param {Boolean} mode
 * @return {Boolean}
 */
Handle<Value> MysqlConnection::AutoCommitSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_CONNECTED;

    REQ_BOOL_ARG(0, autocomit)

    if (mysql_autocommit(conn->_conn, autocomit)) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

/**
 * Changes the user and causes the database to become the default
 *
 * @param {String} user
 * @param {String|null} password
 * @param {String|null} database
 * @return {Boolean}
 */
Handle<Value> MysqlConnection::ChangeUserSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_CONNECTED;

    REQ_STR_ARG(0, user)

    // TODO(Sannis): Check logic
    if ( (args.Length() < 2) || (!args[0]->IsString()) ||
         (!args[1]->IsString()) ) {
        return THRTYPEEXC("Must give at least user and password as arguments");
    }
    String::Utf8Value password(args[1]->ToString());

    if ( (args.Length() == 3) && (!args[2]->IsString()) ) {
        return THRTYPEEXC("Must give string value as third argument, dbname");
    }
    String::Utf8Value dbname(args[2]->ToString());

    bool r = mysql_change_user(conn->_conn,
                               *user,
                               args[1]->IsString() ? *password : NULL,
                               args[2]->IsString() ? *dbname : NULL);

    if (r) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

/**
 * Commits the current transaction
 *
 * @return {Boolean}
 */
Handle<Value> MysqlConnection::CommitSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_CONNECTED;

    if (mysql_commit(conn->_conn)) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

/**
 * EIO wrapper functions for MysqlConnection::Connect
 */
async_rtn MysqlConnection::EIO_After_Connect(uv_work_t *req) {
    HandleScope scope;
    
    struct connect_request *conn_req = (struct connect_request *)(req->data);

    Local<Value> argv[1];

    if (!conn_req->ok) {
        unsigned int error_string_length =
                                    strlen(conn_req->conn->connect_error) + 25;
        char* error_string = new char[error_string_length];
        snprintf(error_string, error_string_length, "Connection error #%d: %s",
               conn_req->errno, conn_req->error);

        argv[0] = V8EXC(error_string);
        delete[] error_string;
    } else {
        argv[0] = Local<Value>::New(Null());
    }

    TryCatch try_catch;

    conn_req->callback->Call(Context::GetCurrent()->Global(), 1, argv);

    if (try_catch.HasCaught()) {
        node::FatalException(try_catch);
    }

    conn_req->callback.Dispose();
    
    conn_req->conn->Unref();
    
    delete conn_req;

    RETURN_ASYNC_AFTER
}

async_rtn MysqlConnection::EIO_Connect(uv_work_t *req) {
    struct connect_request *conn_req = (struct connect_request *)(req->data);

    conn_req->ok = conn_req->conn->Connect(
                        conn_req->hostname ? **(conn_req->hostname) : NULL,
                        conn_req->user ? **(conn_req->user) : NULL,
                        conn_req->password ? **(conn_req->password) : NULL,
                        conn_req->dbname ? **(conn_req->dbname) : NULL,
                        conn_req->port,
                        conn_req->socket ? **(conn_req->socket) : NULL,
                        conn_req->flags
                    ) ? true : false;

    if (!conn_req->ok) {
        conn_req->errno = conn_req->conn->connect_errno;
        conn_req->error = conn_req->conn->connect_error;
    }

    delete conn_req->hostname;
    delete conn_req->user;
    delete conn_req->password;
    delete conn_req->socket;

    RETURN_ASYNC
}

/**
 * Opens a new connection to the MySQL server
 *
 * @param {String|null} hostname
 * @param {String|null} user
 * @param {String|null} password
 * @param {String|null} database
 * @param {Integer|null} port
 * @param {String|null} socket
 * @param {Function(error)} callback
 */
Handle<Value> MysqlConnection::Connect(const Arguments& args) {
    HandleScope scope;
    
    REQ_FUN_ARG(args.Length() - 1, callback);

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    connect_request *conn_req = new connect_request;
    
    conn_req->callback = Persistent<Function>::New(callback);
    conn_req->conn = conn;
    conn->Ref();

    conn_req->hostname = args.Length() > 1 && args[0]->IsString() ?
        new String::Utf8Value(args[0]->ToString()) : NULL;
    conn_req->user = args.Length() > 2 && args[1]->IsString() ?
        new String::Utf8Value(args[1]->ToString()) : NULL;
    conn_req->password = args.Length() > 3 && args[2]->IsString() ?
        new String::Utf8Value(args[2]->ToString()) : NULL;
    conn_req->dbname = args.Length() > 4 && args[3]->IsString() ?
        new String::Utf8Value(args[3]->ToString()) : NULL;
    conn_req->port = args.Length() > 5 && args[4]->IsUint32() ?
                            args[4]->Uint32Value() : 0;
    conn_req->socket = args.Length() > 6 && args[5]->IsString() ?
        new String::Utf8Value(args[5]->ToString()) : NULL;
    conn_req->flags = args.Length() > 7 && args[6]->IsUint32() ?
                            args[6]->Uint32Value() : 0;

    BEGIN_ASYNC(conn_req, EIO_Connect, EIO_After_Connect)

    return Undefined();
}

/**
 * Opens a new connection to the MySQL server
 *
 * @param {String|null} hostname
 * @param {String|null} user
 * @param {String|null} password
 * @param {String|null} database
 * @param {Integer|null} port
 * @param {String|null} socket
 * @return {Boolean}
 */
Handle<Value> MysqlConnection::ConnectSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    if (conn->_conn) {
        return THREXC("Already initialized. "
                       "Use conn.realConnectSync() after conn.initSync()");
    }

    String::Utf8Value hostname(args[0]->ToString());
    String::Utf8Value user(args[1]->ToString());
    String::Utf8Value password(args[2]->ToString());
    String::Utf8Value dbname(args[3]->ToString());
    uint32_t port = args[4]->Uint32Value();
    String::Utf8Value socket(args[5]->ToString());
    uint64_t flags = args[6]->Uint32Value();

    bool r = conn->Connect(args[0]->IsString() ? *hostname : NULL,
                           args[1]->IsString() ? *user     : NULL,
                           args[2]->IsString() ? *password : NULL,
                           args[3]->IsString() ? *dbname   : NULL,
                           args[4]->IsUint32() ? port      : 0,
                           args[5]->IsString() ? *socket   : NULL,
                           args[6]->IsUint32() ? flags     : 0);

    if (!r) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

/**
 * Returns current connected status
 *
 * @return {Boolean}
 */
Handle<Value> MysqlConnection::ConnectedSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    return scope.Close(conn->connected ? True() : False());
}

/**
 * Closes database connection
 */
Handle<Value> MysqlConnection::CloseSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_CONNECTED;

    conn->Close();

    return Undefined();
}

/**
 * Performs debugging operations
 *
 * @param {String} debug message
 */
Handle<Value> MysqlConnection::DebugSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_CONNECTED;

    REQ_STR_ARG(0, debug)

    mysql_debug(*debug);

    return Undefined();
}

/**
 * Dump debugging information into the log
 *
 * @return {Boolean}
 */
Handle<Value> MysqlConnection::DumpDebugInfoSync(const Arguments& args) {
    HandleScope scope;
    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_CONNECTED;

    return scope.Close(mysql_dump_debug_info(conn->_conn) ? False() : True());
}

/**
 * Returns the last error code
 *
 * @return {Integer}
 */
Handle<Value> MysqlConnection::ErrnoSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_CONNECTED;

    return scope.Close(Integer::NewFromUnsigned(mysql_errno(conn->_conn)));
}

/**
 * Returns a string description of the last error
 *
 * @return {String}
 */
Handle<Value> MysqlConnection::ErrorSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_CONNECTED;

    const char *error = mysql_error(conn->_conn);

    return scope.Close(V8STR(error));
}

/**
 * Escapes special characters in a string for use in an SQL statement,
 * taking into account the current charset of the connection
 *
 * @param {String} string
 * @return {String}
 */
Handle<Value> MysqlConnection::EscapeSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_CONNECTED;

    REQ_STR_ARG(0, str)

    unsigned int len = static_cast<unsigned int>(str.length());
    char *result = new char[2*len + 1];
    if (!result) {
        V8::LowMemoryNotification();
        return THREXC("Not enough memory");
    }

    mysql_real_escape_string(conn->_conn, result, *str, len);
    Local<Value> js_result = V8STR(result);

    delete[] result;

    return scope.Close(js_result);
}

/**
 * Returns the number of columns for the most recent query
 *
 * @return {Integer}
 */
Handle<Value> MysqlConnection::FieldCountSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_CONNECTED;

    return scope.Close(
                Integer::NewFromUnsigned(
                    mysql_field_count(conn->_conn)));
}

/**
 * Returns a character set object
 *
 * @return {Object}
 */
Handle<Value> MysqlConnection::GetCharsetSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_CONNECTED;

    MY_CHARSET_INFO cs;

    mysql_get_character_set_info(conn->_conn, &cs);

    Local<Object> js_result = Object::New();

    js_result->Set(V8STR("charset"), V8STR(cs.csname ? cs.csname : ""));
    js_result->Set(V8STR("collation"), V8STR(cs.name ? cs.name : ""));
    js_result->Set(V8STR("dir"), V8STR(cs.dir ? cs.dir : ""));
    js_result->Set(V8STR("min_length"), Integer::NewFromUnsigned(cs.mbminlen));
    js_result->Set(V8STR("max_length"), Integer::NewFromUnsigned(cs.mbmaxlen));
    js_result->Set(V8STR("number"), Integer::NewFromUnsigned(cs.number));
    js_result->Set(V8STR("state"), Integer::NewFromUnsigned(cs.state));
    js_result->Set(V8STR("comment"), V8STR(cs.comment ? cs.comment : ""));

    return scope.Close(js_result);
}

/**
 * Returns a character set name
 *
 * @return {String}
 */
Handle<Value> MysqlConnection::GetCharsetNameSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_CONNECTED;

    return scope.Close(V8STR(mysql_character_set_name(conn->_conn)));
}

/**
 * Returns the MySQL client version and information
 *
 * @return {Object}
 */
Handle<Value> MysqlConnection::GetClientInfoSync(const Arguments& args) {
    HandleScope scope;

    Local<Object> js_result = Object::New();

    js_result->Set(V8STR("client_info"),
                   V8STR(mysql_get_client_info()));
    js_result->Set(V8STR("client_version"),
                   Integer::NewFromUnsigned(mysql_get_client_version()));

    return scope.Close(js_result);
}

/**
 * Returns the MySQL client, server, host and protocol version and information
 *
 * @return {Object}
 */
Handle<Value> MysqlConnection::GetInfoSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_CONNECTED;

    Local<Object> js_result = Object::New();

    js_result->Set(V8STR("client_info"),
                   V8STR(mysql_get_client_info()));
    js_result->Set(V8STR("client_version"),
                   Integer::NewFromUnsigned(mysql_get_client_version()));
    js_result->Set(V8STR("server_info"),
                   V8STR(mysql_get_server_info(conn->_conn)));
    js_result->Set(V8STR("server_version"),
                   Integer::NewFromUnsigned(
                       mysql_get_server_version(conn->_conn)));
    js_result->Set(V8STR("host_info"),
                   V8STR(mysql_get_host_info(conn->_conn)));
    js_result->Set(V8STR("proto_info"),
                   Integer::NewFromUnsigned(mysql_get_proto_info(conn->_conn)));

    return scope.Close(js_result);
}

/**
 * Retrieves information about the most recently executed query
 *
 * @return {String}
 */
Handle<Value> MysqlConnection::GetInfoStringSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_CONNECTED;

    const char *info = mysql_info(conn->_conn);

    return scope.Close(V8STR(info ? info : ""));
}

/**
 * Gets result of SHOW WARNINGS
 *
 * @return {Array}
 */
Handle<Value> MysqlConnection::GetWarningsSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_CONNECTED;

    MYSQL_RES *result;
    MYSQL_ROW row;
    int i = 0;

    Local<Object> js_warning = Object::New();
    Local<Array> js_result = Array::New();

    if (mysql_warning_count(conn->_conn)) {
        if (mysql_real_query(conn->_conn, "SHOW WARNINGS", 13) == 0) {
            result = mysql_store_result(conn->_conn);

            while ((row = mysql_fetch_row(result))) {
                js_warning->Set(V8STR("errno"), V8STR(row[1])->ToInteger());

                js_warning->Set(V8STR("reason"), V8STR(row[2]));

                js_result->Set(Integer::New(i), js_warning);

                i++;
            }

            mysql_free_result(result);
        }
    }

    return scope.Close(js_result);
}

/**
 * Initializes MysqlConnection
 *
 * @return {Boolean}
 */
Handle<Value> MysqlConnection::InitSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    if (conn->_conn) {
        return THREXC("Already initialized");
    }

    conn->_conn = mysql_init(NULL);

    if (!conn->_conn) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

/**
 * Initializes a statement and returns it
 *
 * @return {MysqlStatement|Boolean}
 */
Handle<Value> MysqlConnection::InitStatementSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQL_STMT *my_statement = mysql_stmt_init(conn->_conn);

    if (!my_statement) {
        return scope.Close(False());
    }

    Local<Value> arg = External::New(my_statement);
    Persistent<Object> js_result(MysqlStatement::constructor_template->
                             GetFunction()->NewInstance(1, &arg));

    return scope.Close(js_result);
}

/**
 * Returns the auto generated id used in the last query
 *
 * @return {Integer}
 */
Handle<Value> MysqlConnection::LastInsertIdSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_CONNECTED;

    MYSQL_RES *result;
    my_ulonglong insert_id = 0;

    if ( (result = mysql_store_result(conn->_conn)) == 0 &&
         mysql_field_count(conn->_conn) == 0 &&
         mysql_insert_id(conn->_conn) != 0) {
        insert_id = mysql_insert_id(conn->_conn);
    }

    return scope.Close(Integer::New(insert_id));
}

/**
 * Checks if there are any more query results from a multi query
 *
 * @return {Boolean}
 */
Handle<Value> MysqlConnection::MultiMoreResultsSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_CONNECTED;

    if (mysql_more_results(conn->_conn)) {
        return scope.Close(True());
    }

    return scope.Close(False());
}

/**
 * Prepare next result from multi_query
 *
 * @return {Boolean}
 */
Handle<Value> MysqlConnection::MultiNextResultSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_CONNECTED;

    if (!mysql_more_results(conn->_conn)) {
        return THREXC("There is no next result set."
                        "Please, call MultiMoreResultsSync() to check "
                        "whether to call this function/method");
    }

    if (!mysql_next_result(conn->_conn)) {
        return scope.Close(True());
    }

    return scope.Close(False());
}

/**
 * Performs a multi_query on the database
 *
 * @param {String} multi_query
 * @return {Boolean}
 */
Handle<Value> MysqlConnection::MultiRealQuerySync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    REQ_STR_ARG(0, query)

    MYSQLCONN_MUSTBE_CONNECTED;

    MYSQLCONN_ENABLE_MQ;
    unsigned int query_len = static_cast<unsigned int>(query.length());
    if (mysql_real_query(conn->_conn, *query, query_len) != 0) {
        MYSQLCONN_DISABLE_MQ;
        return scope.Close(False());
    }
    MYSQLCONN_DISABLE_MQ;

    return scope.Close(True());
}

/**
 * Pings a server connection,
 * or tries to reconnect if the connection has gone down
 *
 * @return {Boolean}
 */
Handle<Value> MysqlConnection::PingSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_CONNECTED;

    if (mysql_ping(conn->_conn)) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

/**
 * EIO wrapper functions for MysqlConnection::Query
 */
async_rtn MysqlConnection::EIO_After_Query(uv_work_t *req) {
    HandleScope scope;

    struct query_request *query_req = (struct query_request *)(req->data);

    // We can't use const int argc here because argv is used
    // for both MysqlResult creation and callback call
    int argc = 1; // node.js convention, there is always at least one argument for callback
    Local<Value> argv[3];

    if (!query_req->ok) {
        unsigned int error_string_length = strlen(query_req->error) + 20;
        char* error_string = new char[error_string_length];
        snprintf(error_string, error_string_length, "Query error #%d: %s",
                 query_req->errno, query_req->error);

        argv[0] = V8EXC(error_string);
        delete[] error_string;
    } else {
        if (query_req->have_result_set) {
            argv[0] = External::New(query_req->conn->_conn);
            argv[1] = External::New(query_req->my_result);
            argv[2] = Integer::NewFromUnsigned(query_req->field_count);
            Persistent<Object> js_result(MysqlResult::constructor_template->
                                     GetFunction()->NewInstance(3, argv));

            argv[1] = Local<Object>::New(js_result);
        } else {
            Local<Object> js_result = Object::New();
            js_result->Set(V8STR("affectedRows"),
                           Integer::New(query_req->affected_rows));
            js_result->Set(V8STR("insertId"),
                           Integer::New(query_req->insert_id));
            argv[1] = Local<Object>::New(js_result);
        }
        argc = 2;
        argv[0] = Local<Value>::New(Null());
    }

    if (query_req->callback->IsFunction()) {
        TryCatch try_catch;

        Persistent<Function>::Cast(query_req->callback)->
                              Call(Context::GetCurrent()->Global(), argc, argv);

        if (try_catch.HasCaught()) {
            node::FatalException(try_catch);
        }

        query_req->callback.Dispose();
    }

    query_req->conn->Unref();

    delete[] query_req->query;
    delete query_req;

    RETURN_ASYNC_AFTER
}

async_rtn MysqlConnection::EIO_Query(uv_work_t *req) {
    struct query_request *query_req = (struct query_request *)(req->data);

    MysqlConnection *conn = query_req->conn;

    if (!conn->_conn) {
        query_req->ok = false;
        RETURN_ASYNC
    }

    MYSQLCONN_DISABLE_MQ;

    pthread_mutex_lock(&conn->query_lock);
    int r = mysql_real_query(conn->_conn, query_req->query, query_req->query_len);
    int errno = mysql_errno(conn->_conn);
    if (r != 0 || errno != 0) {
        // Query error
        query_req->ok = false;
        query_req->errno = errno;
        query_req->error = mysql_error(conn->_conn);
    } else {
        query_req->ok = true;

        MYSQL_RES *my_result = mysql_store_result(conn->_conn);

        query_req->field_count = mysql_field_count(conn->_conn);

        if (my_result) {
            // Valid result set (may be empty, of cause)
            query_req->have_result_set = true;
            query_req->my_result = my_result;
        } else {
            if (query_req->field_count == 0) {
                // No result set - not a SELECT, SHOW, DESCRIBE or EXPLAIN
                query_req->have_result_set = false;
                // UPDATE or DELETE?
                query_req->affected_rows = mysql_affected_rows(conn->_conn);
                // INSERT?
                query_req->insert_id = mysql_insert_id(conn->_conn);
            } else {
                // Result store error
                query_req->ok = false;
                query_req->errno = errno;
                query_req->error = mysql_error(conn->_conn);
            }
        }
    }
    pthread_mutex_unlock(&conn->query_lock);
    
    RETURN_ASYNC
}

/**
 * Performs a query on the database
 *
 * @param {String} query
 * @param {Function(error, result)} callback
 */
Handle<Value> MysqlConnection::Query(const Arguments& args) {
    HandleScope scope;

    REQ_STR_ARG(0, query);
    OPTIONAL_FUN_ARG(1, callback);

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_CONNECTED;

    query_request *query_req = new query_request;

    unsigned int query_len = static_cast<unsigned int>(query.length());
    
    query_req->query = new char[query_len + 1];
    query_req->query_len = query_len;

    // Copy query from V8 value to buffer
    memcpy(query_req->query, *query, query_len);
    query_req->query[query_len] = '\0';

    query_req->callback = Persistent<Value>::New(callback);
    query_req->conn = conn;
    conn->Ref();

    BEGIN_ASYNC(query_req, EIO_Query, EIO_After_Query)

    return Undefined();
}

/**
 * EV wrapper functions for MysqlConnection::QuerySend
 */
void MysqlConnection::EV_After_QuerySend(EV_P_ ev_io *io_watcher, int revents) {
    HandleScope scope;

    // Fake uv_work_t struct for EIO_After_Query call
    uv_work_t *_req = new uv_work_t;
    _req->data = io_watcher->data;

    // Stop io_watcher
    ev_io_stop(EV_A_ io_watcher);

    struct query_request *query_req = (struct query_request *)(io_watcher->data);

    MysqlConnection *conn = query_req->conn;

    // The query part, mostly same with EIO_Query
    // TODO merge the same parts with EIO_Query
    int r = mysql_read_query_result(conn->_conn);
    int errno = mysql_errno(conn->_conn);
    if (r != 0 || errno != 0) {
        // Query error
        query_req->ok = false;
        query_req->errno = errno;
        query_req->error = mysql_error(conn->_conn);
    } else {
        query_req->ok = true;

        MYSQL_RES *my_result = mysql_store_result(conn->_conn);

        query_req->field_count = mysql_field_count(conn->_conn);

        if (my_result) {
            // Valid result set (may be empty, of cause)
            query_req->have_result_set = true;
            query_req->my_result = my_result;
        } else {
            if (query_req->field_count == 0) {
                // No result set - not a SELECT, SHOW, DESCRIBE or EXPLAIN
                query_req->have_result_set = false;
                // UPDATE or DELETE?
                query_req->affected_rows = mysql_affected_rows(query_req->conn->_conn);
                // INSERT?
                query_req->insert_id = mysql_insert_id(query_req->conn->_conn);
            } else {
                // Result store error
                query_req->ok = false;
                query_req->errno = errno;
                query_req->error = mysql_error(query_req->conn->_conn);
            }
        }
    }

    // The callback part, just call the existing code
    EIO_After_Query(_req);

    // Don't forget to delete io_watcher
    delete io_watcher;
}

/**
 * Performs a query on the database
 *
 * @param {String} querySend
 * @param {Function(error, result)} callback
 */
Handle<Value> MysqlConnection::QuerySend(const Arguments& args) {
    HandleScope scope;

    REQ_STR_ARG(0, query);
    OPTIONAL_FUN_ARG(1, callback);

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_CONNECTED;

    query_request *query_req = new query_request;

    unsigned int query_len = static_cast<unsigned int>(query.length());
    query_req->query = new char[query_len + 1];

    // Copy query from V8 var to buffer
    memcpy(query_req->query, *query, query_len);
    query_req->query[query_len] = '\0';

    query_req->callback = Persistent<Value>::New(callback);
    query_req->conn = conn;
    conn->Ref();

    // Send query
    mysql_send_query(conn->_conn, query_req->query, query_len + 1);

    // Init ev watcher
    ev_io* io_watcher = new ev_io;
    io_watcher->data = query_req;

    BEGIN_EV_IO_WATCH(io_watcher, EV_After_QuerySend, conn->_conn->net.fd, EV_READ)

    return Undefined();
}


/**
 * Performs a query on the database
 *
 * @param {String} query
 * @param {MysqlResult} result
 */
Handle<Value> MysqlConnection::QuerySync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    REQ_STR_ARG(0, query)

    MYSQLCONN_MUSTBE_CONNECTED;

    MYSQLCONN_DISABLE_MQ;

    MYSQL_RES *my_result = NULL;
    unsigned int field_count;
    
    unsigned int query_len = static_cast<unsigned int>(query.length());

    // Only one query can be executed on a connection at a time
    pthread_mutex_lock(&conn->query_lock);

    int r = mysql_real_query(conn->_conn, *query, query_len);
    if (r == 0) {
        my_result = mysql_store_result(conn->_conn);
        field_count = mysql_field_count(conn->_conn);
    }

    pthread_mutex_unlock(&conn->query_lock);

    if (r != 0) {
        // Query error
        return scope.Close(False());
    }

    if (!my_result) {
        if (field_count == 0) {
            // No result set - not a SELECT, SHOW, DESCRIBE or EXPLAIN
            return scope.Close(True());
        } else {
            // Error
            return scope.Close(False());
        }
    }

    const int argc = 3;
    Local<Value> argv[argc];
    argv[0] = External::New(conn->_conn);
    argv[1] = External::New(my_result);
    argv[2] = Integer::NewFromUnsigned(field_count);
    Persistent<Object> js_result(MysqlResult::constructor_template->
                             GetFunction()->NewInstance(argc, argv));

    return scope.Close(js_result);
}

/**
 * Rolls back current transaction
 *
 * @return {Boolean}
 */
Handle<Value> MysqlConnection::RollbackSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_CONNECTED;

    if (mysql_rollback(conn->_conn)) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

/**
 * Opens a new connection to the MySQL server
 *
 * @param {String|null} hostname
 * @param {String|null} user
 * @param {String|null} password
 * @param {String|null} database
 * @param {Integer|null} port
 * @param {String|null} socket
 * @return {Boolean}
 */
Handle<Value> MysqlConnection::RealConnectSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_INITIALIZED;

    String::Utf8Value hostname(args[0]->ToString());
    String::Utf8Value user(args[1]->ToString());
    String::Utf8Value password(args[2]->ToString());
    String::Utf8Value dbname(args[3]->ToString());
    uint32_t port = args[4]->Uint32Value();
    String::Utf8Value socket(args[5]->ToString());
    uint64_t flags = args[6]->Uint32Value();

    bool r = conn->RealConnect(args[0]->IsString() ? *hostname : NULL,
                               args[1]->IsString() ? *user     : NULL,
                               args[2]->IsString() ? *password : NULL,
                               args[3]->IsString() ? *dbname   : NULL,
                               args[4]->IsUint32() ? port      : 0,
                               args[5]->IsString() ? *socket   : NULL,
                               args[6]->IsUint32() ? flags     : 0);

    if (!r) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

/**
 * Executes an SQL query
 *
 * @param {String} query
 * @return {Boolean}
 */
Handle<Value> MysqlConnection::RealQuerySync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    REQ_STR_ARG(0, query)

    MYSQLCONN_MUSTBE_CONNECTED;

    MYSQLCONN_DISABLE_MQ;

    unsigned int query_len = static_cast<unsigned int>(query.length());

    pthread_mutex_lock(&conn->query_lock);
    int r = mysql_real_query(conn->_conn, *query, query_len);
    pthread_mutex_unlock(&conn->query_lock);

    if (r != 0) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

/**
 * Selects the default database for database queries
 *
 * @param {String} database
 * @return {Boolean}
 */
Handle<Value> MysqlConnection::SelectDbSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_CONNECTED;

    REQ_STR_ARG(0, dbname)

    if (mysql_select_db(conn->_conn, *dbname)) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

/**
 * Sets the default client character set
 *
 * @param {String} charset
 * @return {Boolean}
 */
Handle<Value> MysqlConnection::SetCharsetSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_CONNECTED;

    REQ_STR_ARG(0, charset)

    if (mysql_set_character_set(conn->_conn, *charset)) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

/**
 * Sets options
 *
 * @param {Integer} option key
 * @param {Integer|String} option value
 * @return {Boolean}
 */
Handle<Value> MysqlConnection::SetOptionSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    REQ_INT_ARG(0, option_integer_key);
    mysql_option option_key = static_cast<mysql_option>(option_integer_key);
    int r = 1;

    switch (option_key) {
        case MYSQL_OPT_CONNECT_TIMEOUT:
        case MYSQL_OPT_PROTOCOL:
        case MYSQL_OPT_READ_TIMEOUT:
        case MYSQL_OPT_WRITE_TIMEOUT:
        case MYSQL_OPT_RECONNECT:
        case MYSQL_OPT_COMPRESS:
        case MYSQL_OPT_SSL_VERIFY_SERVER_CERT:
        case MYSQL_REPORT_DATA_TRUNCATION:
        case MYSQL_SECURE_AUTH:
            {
            REQ_INT_ARG(1, option_integer_value);
            r = mysql_options(conn->_conn,
                              option_key,
                              static_cast<const char *>(
                                static_cast<const void *>(
                                  &option_integer_value)));
            // MYSQL_OPT_RECONNECT option is modified by mysql_real_connect
            // due to bug in MySQL < 5.1.6
            // Save it state and repeat mysql_options after connect
            // See issue #66
#if MYSQL_VERSION_ID >= 50013 && MYSQL_VERSION_ID < 50106
            if (!r && (option_key == MYSQL_OPT_RECONNECT)) {
                conn->opt_reconnect = option_integer_value;
            }
#endif
            }
            break;
        case MYSQL_READ_DEFAULT_FILE:
        case MYSQL_READ_DEFAULT_GROUP:
        case MYSQL_INIT_COMMAND:
        case MYSQL_SET_CHARSET_NAME:
        case MYSQL_SET_CHARSET_DIR:
            {
            REQ_STR_ARG(1, option_string_value);
            r = mysql_options(conn->_conn, option_key, *option_string_value);
            }
            break;
        case MYSQL_OPT_LOCAL_INFILE:
            return THREXC("This option isn't implemented yet");
            break;
        case MYSQL_OPT_NAMED_PIPE:
        case MYSQL_SHARED_MEMORY_BASE_NAME:
            return THREXC("This option isn't used because Windows");
            break;
        case MYSQL_OPT_GUESS_CONNECTION:
        case MYSQL_SET_CLIENT_IP:
        case MYSQL_OPT_USE_EMBEDDED_CONNECTION:
        case MYSQL_OPT_USE_REMOTE_CONNECTION:
            return THREXC("This option isn't used because not embedded");
            break;
        case MYSQL_OPT_USE_RESULT:
        default:
            return THREXC("This option isn't supported by MySQL");
    }

    if (r) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

/**
 * Sets SSL options
 * Used for establishing secure connections
 *
 * @param {String} key file
 * @param {String} certificate file
 * @param {String} certificate authority file
 * @param {String} CA certificates in PEM directory
 * @param {String} allowable ciphers
 */
Handle<Value> MysqlConnection::SetSslSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_CONNECTED;

    REQ_STR_ARG(0, key)
    REQ_STR_ARG(1, cert)
    REQ_STR_ARG(2, ca)
    REQ_STR_ARG(3, capath)
    REQ_STR_ARG(4, cipher)

    mysql_ssl_set(conn->_conn, *key, *cert, *ca, *capath, *cipher);

    return scope.Close(Undefined());
}

/**
 * Returns the SQLSTATE error from previous MySQL operation
 *
 * @return {String}
 */
Handle<Value> MysqlConnection::SqlStateSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_CONNECTED;

    return scope.Close(V8STR(mysql_sqlstate(conn->_conn)));
}

/**
 * Gets the current system status
 *
 * @return {String}
 */
Handle<Value> MysqlConnection::StatSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_CONNECTED;

    const char *stat = mysql_stat(conn->_conn);

    return scope.Close(V8STR(stat ? stat : ""));
}

/**
 * Transfers a result set from the last query
 *
 * @return {MysqlResult}
 */
Handle<Value> MysqlConnection::StoreResultSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    if (!mysql_field_count(conn->_conn)) {
        /* no result set - not a SELECT, SHOW, DESCRIBE or EXPLAIN, */
        return scope.Close(True());
    }

    MYSQL_RES *my_result = mysql_store_result(conn->_conn);

    if (!my_result) {
        return scope.Close(False());
    }

    const int argc = 3;
    Local<Value> argv[argc];
    argv[0] = External::New(conn->_conn);
    argv[1] = External::New(my_result);
    argv[2] = Integer::NewFromUnsigned(mysql_field_count(conn->_conn));
    Persistent<Object> js_result(MysqlResult::constructor_template->
                             GetFunction()->NewInstance(argc, argv));

    return scope.Close(js_result);
}

/**
 * Returns the thread ID for the current connection
 *
 * @return {Integer}
 */
Handle<Value> MysqlConnection::ThreadIdSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_CONNECTED;

    uint64_t thread_id = mysql_thread_id(conn->_conn);

    return scope.Close(Integer::New(thread_id));
}

/**
 * Returns whether thread safety is given or not
 *
 * @return {Boolean}
 */
Handle<Value> MysqlConnection::ThreadSafeSync(const Arguments& args) {
    HandleScope scope;

    if (mysql_thread_safe()) {
        return scope.Close(True());
    } else {
        return scope.Close(False());
    }
}

/**
 * Initiates a result set retrieval
 *
 * @return {MysqlResult}
 */
Handle<Value> MysqlConnection::UseResultSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    if (!mysql_field_count(conn->_conn)) {
        /* no result set - not a SELECT, SHOW, DESCRIBE or EXPLAIN, */
        return scope.Close(True());
    }

    MYSQL_RES *my_result = mysql_use_result(conn->_conn);

    if (!my_result) {
        return scope.Close(False());
    }

    const int argc = 3;
    Local<Value> argv[argc];
    argv[0] = External::New(conn->_conn);
    argv[1] = External::New(my_result);
    argv[2] = Integer::NewFromUnsigned(mysql_field_count(conn->_conn));
    Persistent<Object> js_result(MysqlResult::constructor_template->
                             GetFunction()->NewInstance(argc, argv));

    return scope.Close(js_result);
}

/**
 * Returns the number of warnings from the last query for the given link
 *
 * @return {Integer}
 */
Handle<Value> MysqlConnection::WarningCountSync(const Arguments& args) {
    HandleScope scope;

    MysqlConnection *conn = OBJUNWRAP<MysqlConnection>(args.This());

    MYSQLCONN_MUSTBE_CONNECTED;

    uint32_t warning_count = mysql_warning_count(conn->_conn);

    return scope.Close(Integer::NewFromUnsigned(warning_count));
}

